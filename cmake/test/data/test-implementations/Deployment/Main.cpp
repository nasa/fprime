bool good_implementation(); // Base implementation selection worked
bool good_override(); // Override implementation selection worked

// No operation executable
int main(int argc, char** argv) {
    return static_cast<int>(good_implementation() && good_override());
}
