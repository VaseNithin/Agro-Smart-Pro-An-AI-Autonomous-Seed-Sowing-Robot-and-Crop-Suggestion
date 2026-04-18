#ifndef MODEL_H
#define MODEL_H

#include <Arduino.h>

String predictCrop(int n, int p, int k, bool unstable) {

    if (n < 5 && p < 5 && k < 5)
        return "Sensor Error: No Contact";

    if (unstable)
        return "Data Unstable: Retry Analysis";

    // High Nitrogen
    if (n > 75) {
        if (p > 50 && k > 45) return "Rice or Jute";
        if (p < 40 && k < 40) return "Coffee or Banana";
        return "Maize or Cotton";
    }

    // High Phosphorus & Potassium
    if (p > 75 && k > 75) {
        if (n < 40) return "Apple or Grapes";
        return "Papaya or Watermelon";
    }

    // Low Nitrogen
    if (n < 50) {
        if (p > 50) return "Black Gram or Lentils";
        if (p < 30 && k < 30) return "Pigeon Peas or Mango";
        return "Pomegranate or Orange";
    }

    // Balanced Soil
    if (n > 50 && n <= 75) {
        if (k > 50) return "Muskmelon or Cantaloupe";
        return "Coconut or Arecanut";
    }

    return "General Crop: Coconut";
}

#endif
