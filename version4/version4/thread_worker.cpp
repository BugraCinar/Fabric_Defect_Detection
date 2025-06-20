#include "thread_worker.h"
#include "readPPM_binary.h"
#include "globals.h"
#include <fstream>
#include <iostream>
#include <mutex>  
#include <cmath>  
#include <immintrin.h> // avx2 simd 

extern std::mutex outputMutex;  


constexpr int blockSize = 5; 
constexpr double blockThreshold = 1000.0;  
constexpr double defectRatioThreshold = 0.01;  


inline double blockDifference(const std::vector<Pixel>& ref, const std::vector<Pixel>& test,
                              int x, int y, int w, int h, int imgW) {
    double total = 0;  
    int count = 0;  

    
    for (int dy = 0; dy < h; ++dy) {
        int row = y + dy; 
        int baseIdx = row * imgW + x;  

        // 8 pikselli
        for (int dx = 0; dx + 7 < w; dx += 8) {
        
            __m256i refVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&ref[baseIdx + dx]));
            __m256i testVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&test[baseIdx + dx]));

            // iki piksel  fark
            __m256i diff = _mm256_subs_epu8(refVec, testVec);  // 8-bit fark
            __m256i sq = _mm256_maddubs_epi16(diff, diff);  // farkların kareleri
            __m256i sum = _mm256_madd_epi16(sq, _mm256_set1_epi16(1));  // toplam farki hesaplama

            alignas(32) int result[8];  // sonuc dizi tanimalama
            _mm256_store_si256(reinterpret_cast<__m256i*>(result), sum);  

           
            for (int i = 0; i < 8; ++i) total += result[i];
            count += 8;  
        }

        // tail islemi
        for (int dx = (w & ~7); dx < w; ++dx) {  
            int idx = baseIdx + dx;
            const Pixel& p1 = ref[idx];  
            const Pixel& p2 = test[idx];  
            int dr = p1.r - p2.r, dg = p1.g - p2.g, db = p1.b - p2.b;  
            total += dr * dr + dg * dg + db * db;  
            ++count;  
        }
    }

 
    return count > 0 ? total / count : 999999.0;
}


void threadAnalyzeWorker(const std::vector<std::filesystem::path>& files,
                         const std::string& referencePath,
                         const std::string& groupName,
                         int threadIndex) {
    std::vector<Pixel> refPixels; 
    int refW = 0, refH = 0;

   
    if (!readPPM_P6(referencePath, refPixels, refW, refH)) {
        std::cerr << "[Thread " << threadIndex << "] Failed to read reference: " << referencePath << "\n";
        return;  
    }

    //append
    std::ofstream perfectOut("outputs/" + groupName + "_perfect_output.txt", std::ios::app);
    std::ofstream defectOut("outputs/" + groupName + "_defect_output.txt", std::ios::app);

    
    for (const auto& file : files) {
        std::vector<Pixel> testPixels;
        int testW = 0, testH = 0;

        
        if (!readPPM_P6(file.string(), testPixels, testW, testH)) continue;
        if (testW != refW || testH != refH) continue;  

        
        int blocksX = refW / blockSize;
        int blocksY = refH / blockSize;
        int totalBlocks = blocksX * blocksY;
        int defectBlocks = 0;  

        
        for (int by = 0; by < blocksY; ++by) {
            for (int bx = 0; bx < blocksX; ++bx) {
                int x = bx * blockSize;  
                int y = by * blockSize;  
                double diff = blockDifference(refPixels, testPixels, x, y, blockSize, blockSize, refW);
                if (diff > blockThreshold) defectBlocks++;  
            }
        }

      
        double defectRatio = static_cast<double>(defectBlocks) / totalBlocks;
        std::string result = (defectRatio > defectRatioThreshold) ? "defection" : "perfect";


        std::lock_guard<std::mutex> lock(outputMutex);
        

        if (result == "perfect") {
            perfectOut << file.filename().string() << " is a perfect fabric.\n";
        } else {
            defectOut << file.filename().string() << " is a defected fabric.\n";
        }
    }

   
    perfectOut.close();
    defectOut.close();
}
