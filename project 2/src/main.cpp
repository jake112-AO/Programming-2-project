#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

struct Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
};

struct Pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

class TGA_Image {
    unsigned char TGA_Multiply(unsigned char p1, unsigned char p2) {
        float n_p1 = p1 / 255.0f;
        float n_p2 = p2 / 255.0f;
        float result = n_p1 * n_p2;
        return static_cast<unsigned char>(result * 255.0f +0.5f);
    }

    unsigned char TGA_Subtract(unsigned char p1, unsigned char p2) {
        int result = static_cast<int>(p1) - static_cast<int>(p2);
        return static_cast<unsigned char>(max(0, result));
    }

    unsigned char TGA_Screen(unsigned char p1, unsigned char p2) {
        float n_p1 = p1 / 255.0f;
        float n_p2 = p2 / 255.0f;
        float result = 1.0f - (1.0f - n_p1) * (1.0f - n_p2);
        return static_cast<unsigned char>(result * 255.0f + 0.5f);
    }

    unsigned char TGA_Overlay(unsigned char p1, unsigned char p2) {
        float n_p1 = p1 / 255.0f;
        float n_p2 = p2 / 255.0f;
        float result;
        if (n_p2 <= 0.5f) {
            result = 2.0f * n_p1 * n_p2;
        } else {
            result = 1.0f - 2.0f * (1.0f - n_p1) * (1.0f - n_p2);
        }
        return static_cast<unsigned char>((result * 255.0f) + 0.5f);
    }
public:
    Header header;
    vector<Pixel> pixels;

    TGA_Image() {}

    TGA_Image(const string& file_name) {
        ReadFromFile(file_name);
    }

    bool ReadFromFile(const string& file_name) {
        ifstream file(file_name, ios::binary);
        if (!file.is_open()) {
            return false;
        }
        file.read(&header.idLength, 1);
        file.read(&header.colorMapType, 1);
        file.read(&header.dataTypeCode, 1);
        file.read(reinterpret_cast<char*>(&header.colorMapOrigin), 2);
        file.read(reinterpret_cast<char*>(&header.colorMapLength), 2);
        file.read(&header.colorMapDepth, 1);
        file.read(reinterpret_cast<char*>(&header.xOrigin), 2);
        file.read(reinterpret_cast<char*>(&header.yOrigin), 2);
        file.read(reinterpret_cast<char*>(&header.width), 2);
        file.read(reinterpret_cast<char*>(&header.height), 2);
        file.read(&header.bitsPerPixel, 1);
        file.read(&header.imageDescriptor, 1);

        if (header.dataTypeCode != 2 || header.bitsPerPixel != 24) {
            file.close();
            return false;
        }

        int n_Pixels = header.width * header.height;
        pixels.resize(n_Pixels);

        for (int i = 0; i < n_Pixels; i++) {
            file.read(reinterpret_cast<char*>(&pixels[i].blue), 1);
            file.read(reinterpret_cast<char*>(&pixels[i].green), 1);
            file.read(reinterpret_cast<char*>(&pixels[i].red), 1);
        }
        file.close();
        return true;
    }

    void WriteToFile(const string& file_name) {
        ofstream file(file_name, ios::binary);
        if (!file.is_open()) {
            cout << "Could not open file " << endl;
            exit(1);
        }
        file.write(&header.idLength, 1);
        file.write(&header.colorMapType, 1);
        file.write(&header.dataTypeCode, 1);
        file.write(reinterpret_cast<char*>(&header.colorMapOrigin), 2);
        file.write(reinterpret_cast<char*>(&header.colorMapLength), 2);
        file.write(&header.colorMapDepth, 1);
        file.write(reinterpret_cast<char*>(&header.xOrigin), 2);
        file.write(reinterpret_cast<char*>(&header.yOrigin), 2);
        file.write(reinterpret_cast<char*>(&header.width), 2);
        file.write(reinterpret_cast<char*>(&header.height), 2);
        file.write(&header.bitsPerPixel, 1);
        file.write(&header.imageDescriptor, 1);

        int n_Pixels = header.width * header.height;
        pixels.resize(n_Pixels);
        for (int i = 0; i < n_Pixels; i++) {
            file.write(reinterpret_cast<char*>(&pixels[i].blue), 1);
            file.write(reinterpret_cast<char*>(&pixels[i].green), 1);
            file.write(reinterpret_cast<char*>(&pixels[i].red), 1);
        }
        file.close();
    }

    void Multiply(const TGA_Image& image) {
        if (header.width != image.header.width || header.height != image.header.height) {
            cout << "Image dimensions do not match" << endl;
            exit(1);
        }
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].red = TGA_Multiply(pixels[i].red, image.pixels[i].red);
            pixels[i].green = TGA_Multiply(pixels[i].green, image.pixels[i].green);
            pixels[i].blue = TGA_Multiply(pixels[i].blue, image.pixels[i].blue);
        }
    }

    void Subtract(const TGA_Image& image) {
        if (header.width != image.header.width || header.height != image.header.height) {
            cout << "Image dimensions do not match" << endl;
            exit(1);
        }
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].red = TGA_Subtract(pixels[i].red, image.pixels[i].red);
            pixels[i].green = TGA_Subtract(pixels[i].green, image.pixels[i].green);
            pixels[i].blue = TGA_Subtract(pixels[i].blue, image.pixels[i].blue);
        }
    }

    void Screen(const TGA_Image& image) {
        if (header.width != image.header.width || header.height != image.header.height) {
            cout << "Image dimensions do not match" << endl;
            exit(1);
        }
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].red = TGA_Screen(pixels[i].red, image.pixels[i].red);
            pixels[i].green = TGA_Screen(pixels[i].green, image.pixels[i].green);
            pixels[i].blue = TGA_Screen(pixels[i].blue, image.pixels[i].blue);
        }
    }

    void Overlay(const TGA_Image& image) {
        if (header.width != image.header.width || header.height != image.header.height) {
            cout << "Image dimensions do not match" << endl;
            exit(1);
        }
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].red = TGA_Overlay(pixels[i].red, image.pixels[i].red);
            pixels[i].green = TGA_Overlay(pixels[i].green, image.pixels[i].green);
            pixels[i].blue = TGA_Overlay(pixels[i].blue, image.pixels[i].blue);
        }
    }

    void Add_TGA_Green(int value) {
        for (int i = 0; i < pixels.size(); i++) {
            int newGreen = static_cast<int>(pixels[i].green) + value;
            pixels[i].green = static_cast<unsigned char>(min(255, max(0,newGreen)));
        }
    }

    void Add_TGA_Red(int value) {
        for (int i = 0; i < pixels.size(); i++) {
            int newRed = static_cast<int>(pixels[i].red) + value;
            pixels[i].red = static_cast<unsigned char>(min(255, max(0,newRed)));
        }
    }
    void Add_TGA_Blue(int value) {
        for (int i = 0; i < pixels.size(); i++) {
            int newBlue = static_cast<int>(pixels[i].blue) + value;
            pixels[i].blue = static_cast<unsigned char>(min(255, max(0,newBlue)));
        }
    }

    void Scale_TGA_Red(float factor) {
        for (int i = 0; i < pixels.size(); i++) {
            float newRed = pixels[i].red * factor;
            int stick = static_cast<int>(newRed + 0.5f);
            pixels[i].red = static_cast<unsigned char>(min(255, max(0,stick)));
        }
    }

    void Scale_TGA_Blue(int factor) {
        for (int i = 0; i < pixels.size(); i++) {
            float newBlue = pixels[i].blue * factor;
            int stick = static_cast<int>(newBlue + 0.5f);
            pixels[i].blue = static_cast<unsigned char>(min(255, max(0,stick)));
        }
    }
    void Scale_TGA_Green(int factor) {
        for (int i = 0; i < pixels.size(); i++) {
            float newGreen = pixels[i].green * factor;
            int stick = static_cast<int>(newGreen + 0.5f);
            pixels[i].green = static_cast<unsigned char>(min(255, max(0,stick)));
        }
    }
    void only_Red() {
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].green = pixels[i].red;
            pixels[i].blue = pixels[i].red;
        }
    }

    void only_Green() {
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].red = pixels[i].green;
            pixels[i].blue = pixels[i].green;
        }
    }

    void only_Blue() {
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].red = pixels[i].blue;
            pixels[i].green = pixels[i].blue;
        }
    }

    void flip() {
        reverse(pixels.begin(), pixels.end());
    }

    void Combine(const TGA_Image& green, const TGA_Image& blue) {
        if (header.width != green.header.width || header.height != green.header.height
            || header.width != blue.header.width || header.height != blue.header.height) {
            cout << "Image dimensions do not match" << endl;
        }
        for (int i = 0; i < pixels.size(); i++) {
            pixels[i].green = green.pixels[i].green;
            pixels[i].blue = blue.pixels[i].blue;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2 || string(argv[1]) == "--help") {
        cout << "Project 2: Image Processing, Spring 2025" << endl << endl;
        cout << "Usage:" << endl;
        cout << "\t./project2.out [output] [firstImage] [method] [...]" << endl;
        return 0;
    }
    if (argc < 3) {
        cout << "Invalid file name." << endl;
        return 1;
    }

    string output_File = argv[1];
    if (output_File.substr(output_File.size() - 4, 4) != ".tga") {
        cout << "Invalid file name." << endl;
        return 1;
    }

    string first_Image = argv[2];
    if (first_Image.substr(first_Image.size() - 4, 4) != ".tga") {
        cout << "Invalid file name." << endl;
        return 1;
    }

    TGA_Image Tracking_Image;
    if (!Tracking_Image.ReadFromFile(first_Image)) {
        cout << "File does not exist." << endl;
        return 1;
    }
    for (int i = 3; i < argc; i++) {
        string method = argv[i];
        if (method == "multiply") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
            }
            string Image_File = argv[++i];
            if (Image_File.substr(Image_File.size() - 4, 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 1;
            }
            TGA_Image image;
            if (!image.ReadFromFile(Image_File)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            Tracking_Image.Multiply(image);
        } else if (method == "subtract") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string Image_File = argv[++i];
            if (Image_File.substr(Image_File.size() - 4, 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 1;
            }
            TGA_Image image;
            if (!image.ReadFromFile(Image_File)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            Tracking_Image.Subtract(image);

        } else if (method == "overlay") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string Image_File = argv[++i];
            if (Image_File.substr(Image_File.size() - 4, 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 1;
            }
            TGA_Image image;
            if (!image.ReadFromFile(Image_File)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            Tracking_Image.Overlay(image);
        } else if (method == "screen") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string Image_File = argv[++i];
            if (Image_File.substr(Image_File.size() - 4, 4) != ".tga") {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            TGA_Image image;
            if (!image.ReadFromFile(Image_File)) {
                cout << "Invalid argument, file does not exist" << endl;
                return 1;
            }
            Tracking_Image.Screen(image);
        } else if (method == "combine") {
            if (i + 2 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string Green_File = argv[++i];
            if (Green_File.substr(Green_File.size() - 4, 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 1;
            }
            string Blue_File = argv[++i];
            if (Blue_File.substr(Blue_File.size() - 4, 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 1;
            }
            TGA_Image green;
            if (!green.ReadFromFile(Green_File)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            TGA_Image blue;
            if (!blue.ReadFromFile(Blue_File)) {
                cout << "Invalid argument, file does not exist." << endl;
                return 1;
            }
            Tracking_Image.Combine(green, blue);
        } else if (method == "flip") {
            Tracking_Image.flip();
        } else if (method == "onlyred") {
            Tracking_Image.only_Red();
        } else if (method == "onlygreen") {
            Tracking_Image.only_Green();
        } else if (method == "onlyblue") {
            Tracking_Image.only_Blue();
        } else if (method == "addred") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string value_string = argv[++i];
            if (!all_of(value_string.begin(), value_string.end(), ::isdigit) && !(value_string[0] == '-'
                && all_of(value_string.begin() + 1, value_string.end(), ::isdigit))){
                cout << "Invalid argument, expected number." << endl;
                return 1;

                }
            int value = atoi(value_string.c_str());
            Tracking_Image.Add_TGA_Red(value);
        } else if (method == "addgreen") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string value_string = argv[++i];
            if (!all_of(value_string.begin(), value_string.end(), ::isdigit) &&
                !(value_string[0] == '-' && all_of(value_string.begin() + 1, value_string.end(), ::isdigit))) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
                }
            int value = atoi(value_string.c_str());
            Tracking_Image.Add_TGA_Green(value);
        } else if (method == "addblue") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string value_string = argv[++i];
            if (!all_of(value_string.begin(), value_string.end(), ::isdigit) &&
                !(value_string[0] == '-' && all_of(value_string.begin() + 1, value_string.end(), ::isdigit))) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
                }
            int value = atoi(value_string.c_str());
            Tracking_Image.Add_TGA_Blue(value);
        } else if (method == "scalered") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string value_string = argv[++i];
            if (!all_of(value_string.begin(), value_string.end(), ::isdigit)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            float value = atof(value_string.c_str());
            Tracking_Image.Scale_TGA_Red(value);
        } else if (method == "scalegreen") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string value_string = argv[++i];
            if (!all_of(value_string.begin(), value_string.end(), ::isdigit)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            float value = atof(value_string.c_str());
            Tracking_Image.Scale_TGA_Green(value);
        } else if (method == "scaleblue") {
            if (i + 1 >= argc) {
                cout << "Missing argument." << endl;
                return 1;
            }
            string value_string = argv[++i];
            if (!all_of(value_string.begin(), value_string.end(), ::isdigit)) {
                cout << "Invalid argument, expected number." << endl;
                return 1;
            }
            float value = atof(value_string.c_str());
            Tracking_Image.Scale_TGA_Blue(value);
        } else {
            cout << "Invalid method name." << endl;
            return 1;
        }
    }
    Tracking_Image.WriteToFile(output_File);
    return 0;
}










    // TGA_Image Layer1_Task1("input/layer1.tga");
    // TGA_Image Pattern1_Task1("input/pattern1.tga");
    // Layer1_Task1.Multiply(Pattern1_Task1);
    // Layer1_Task1.WriteToFile("output/part1.tga");
    //
    // TGA_Image Car_Task2("input/car.tga");
    // TGA_Image Layer2_Task2("input/layer2.tga");
    // Car_Task2.Subtract(Layer2_Task2);
    // Car_Task2.WriteToFile("output/part2.tga");
    //
    // TGA_Image Layer1_Task3("input/layer1.tga");
    // TGA_Image Pattern2_Task3("input/pattern2.tga");
    // Layer1_Task3.Multiply(Pattern2_Task3);
    // TGA_Image text("input/text.tga");
    // Layer1_Task3.Screen(text);
    // Layer1_Task3.WriteToFile("output/part3.tga");
    //
    // TGA_Image Layer2_Task4("input/layer2.tga");
    // TGA_Image Circles_Task4("input/circles.tga");
    // Layer2_Task4.Multiply(Circles_Task4);
    // TGA_Image pattern2_task4("input/pattern2.tga");
    // Layer2_Task4.Subtract(pattern2_task4);
    // Layer2_Task4.WriteToFile("output/part4.tga");
    //
    // TGA_Image Layer1_Task5("input/layer1.tga");
    // TGA_Image Pattern1_Task5("input/pattern1.tga");
    // Layer1_Task5.Overlay(Pattern1_Task5);
    // Layer1_Task5.WriteToFile("output/part5.tga");
    //
    // TGA_Image Car_Task6("input/car.tga");
    // Car_Task6.Add_TGA_Green(200);
    // Car_Task6.WriteToFile("output/part6.tga");
    //
    // TGA_Image Car_Task7("input/car.tga");
    // Car_Task7.Scale_TGA_Red(4.0f);
    // Car_Task7.Scale_TGA_Blue(0.0f);
    // Car_Task7.WriteToFile("output/part7.tga");
    //
    // TGA_Image Car_Task8("input/car.tga");
    // TGA_Image red = Car_Task8;
    // red.only_Red();
    // red.WriteToFile("output/part8_r.tga");
    // TGA_Image green = Car_Task8;
    // green.only_Green();
    // green.WriteToFile("output/part8_g.tga");
    // TGA_Image blue = Car_Task8;
    // blue.only_Blue();
    // blue.WriteToFile("output/part8_b.tga");
    //
    // TGA_Image layer_red("input/layer_red.tga");
    // TGA_Image layer_green("input/layer_green.tga");
    // TGA_Image layer_blue("input/layer_blue.tga");
    // TGA_Image Combined;
    // Combined.header = layer_red.header;
    // Combined.pixels.resize(layer_red.pixels.size());
    // for (int i = 0; i < Combined.pixels.size(); i++) {
    //     Combined.pixels[i].red = layer_red.pixels[i].red;
    //     Combined.pixels[i].green = layer_green.pixels[i].green;
    //     Combined.pixels[i].blue = layer_blue.pixels[i].blue;
    // }
    // Combined.WriteToFile("output/part9.tga");
    //
    // TGA_Image text2("input/text2.tga");
    // text2.flip();
    // text2.WriteToFile("output/part10.tga");
    // return 0;

