#include "Image_Class.h"
#include <iostream>
#include<string>
#include <fstream>
#include<algorithm>
#include<vector>
#include<cmath>
using namespace std;

void invert_colors(string filename)
{

    unsigned char *imageData;
    int width, height, channels;
    imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (imageData == nullptr) {
        cout << "Error loading image." << endl;
    }
    for (int i = 0; i < width * height * channels; i++)
    {
        imageData[i] = 255 - imageData[i];
    }
    stbi_write_jpg(filename.c_str(), width, height, channels, imageData, 100);
    stbi_image_free(imageData);
}


enum Rotation { ROTATE_90, ROTATE_180, ROTATE_270 };
void rotateImage(Image &img, Rotation rotation) {
    if (rotation != ROTATE_90 && rotation != ROTATE_180 && rotation != ROTATE_270) {
        cout << "Invalid angle. Only 90, 180, or 270 degrees rotation is supported." << endl;
        return;
    }

    int width = img.width;
    int height = img.height;
    int channels = img.channels;

    // Calculate new dimensions based on rotation
    int newWidth, newHeight;
    if (rotation == ROTATE_90 || rotation == ROTATE_270) {
        newWidth = height;
        newHeight = width;
    } else {
        newWidth = width;
        newHeight = height;
    }

    // Create temporary buffer for rotated image
    unsigned char* rotatedImageData = new unsigned char[newWidth * newHeight * channels];

    // Perform rotation
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int newX, newY;
            if (rotation == ROTATE_90) {
                newX = newHeight - 1 - y;
                newY = x;
            } else if (rotation == ROTATE_180) {
                newX = newWidth - 1 - x;
                newY = newHeight - 1 - y;
            } else { // angle == 270
                newX = y;
                newY = newWidth - 1 - x;
            }

            for (int c = 0; c < channels; ++c) {
                rotatedImageData[(y * newWidth + x) * channels + c] = img.getPixel(newX, newY, c);
            }
        }
    }

    // Free old image data and update dimensions
    delete[] img.imageData;
    img.imageData = rotatedImageData;
    img.width = newWidth;
    img.height = newHeight;
}

void Rotation1(string filename)
{
    Image img(filename);

    // Ask user for rotation choice
    int rotationChoice;
    cout << "Choose rotation:\n1. 90 degrees\n2. 180 degrees\n3. 270 degrees\n";
    cin >> rotationChoice;
    Rotation rotation;
    while(true)
    {
        if (rotationChoice == 1) {
            rotation = ROTATE_90;
            break;
        }
        else if (rotationChoice == 2) {
            rotation = ROTATE_180;
            break;
        }
        else if (rotationChoice == 3) {
            rotation = ROTATE_270;
            break;
        }
        else {
            cout << "Invalid choice. Please choose 1, 2, or 3." << endl;
            continue;
        }
    }


    // Rotate image
    rotateImage(img, rotation);

    // Save rotated image// Change this to the desired output filename

    img.saveImage("CURRENT_VERSION.jpg");
}

enum Frame {Fancy,Simple};
int colors[6][3] = {
        {255, 0, 0},   // Red
        {255, 255, 0}, // Yellow
        {0, 255, 0},   // Green
        {0, 255, 255}, // Cyan
        {0, 0, 255},   // Blue
        {255, 0, 255}  // Magenta
};

void addFrame(Image &img, int frameSize, int red, int green, int blue,Frame frame) {
    int width = img.width;
    int height = img.height;
    int channels = img.channels;

    // Iterate over the pixels in the border regions to add the frame
    switch (frame)
    {
        case Simple:
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    // Check if the pixel is in the border region
                    if (x < frameSize || x >= width - frameSize || y < frameSize || y >= height - frameSize) {
                        // Set pixel color to frame color
                        img(x, y, 0) = red;
                        img(x, y, 1) = green;
                        img(x, y, 2) = blue;
                    }
                }
            }
        case Fancy:
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    // Check if the pixel is in the border region
                    if (x < frameSize || x >= width - frameSize || y < frameSize || y >= height - frameSize) {
                        int colorIndex = (x + y) % 6;
                        img(x, y, 0) = colors[colorIndex][0]; // Red component
                        img(x, y, 1) = colors[colorIndex][1]; // Green component
                        img(x, y, 2) = colors[colorIndex][2];
                    }
                }
            }
    }
}

void frame (string filename) {
    // Load image (assuming it's named "input_image.jpg")
    cin >> filename;
    Image img(filename);

    // Ask user for frame size and color
    int frameSize;
    int red, green, blue;
    cout << "Enter the frame size: ";
    cin >> frameSize;
    cout << "Enter the RGB values for the frame color (separated by spaces): ";
    cin >> red >> green >> blue;

    // Add a simple frame to the image
    while(true)
    {
        cout<<"1_Simple frame\n"<<"2_Fancy frame\n";
        string framechoice;
        cin>>framechoice;
        Frame frame;
        if(framechoice=="1")
        {
            frame=Simple;
            addFrame(img,frameSize,red,green,blue,frame);
            img.saveImage("CURRENT_VERSION.jpg");
            break;
        }
        else if(framechoice=="2")
        {
            frame=Fancy;
            addFrame(img,frameSize,red,green,blue,frame);
            img.saveImage("CURRENT_VERSION.jpg" );
            break;
        }
        else
        {
            cout<<"Invalid choice ,try again.\n";
            continue;
        }
    }
}


// Function to apply a blur filter to the image
void applyBlurFilter(vector<unsigned char>& imageData, int width, int height, int channels, int blurLevel) {
    // Define the blur kernel based on the blur level
    int kernelSize = 2 * blurLevel + 1;
    float kernelSum = pow(kernelSize, 2);
    vector<vector<float>> blurKernel(kernelSize, vector<float>(kernelSize, 1.0 / kernelSum));

    // Create a temporary vector to store the blurred result
    vector<unsigned char> blurredImage(imageData.size());

    // Iterate over each pixel in the image (excluding border pixels)
    for (int y = blurLevel; y < height - blurLevel; ++y) {
        for (int x = blurLevel; x < width - blurLevel; ++x) {
            // Apply the blur kernel to the pixel and its neighbors
            for (int c = 0; c < channels; ++c) {
                float blurredValue = 0.0;
                for (int ky = -blurLevel; ky <= blurLevel; ++ky) {
                    for (int kx = -blurLevel; kx <= blurLevel; ++kx) {
                        int pixelX = x + kx;
                        int pixelY = y + ky;
                        int kernelX = kx + blurLevel; // Adjust kernel index to match blurKernel
                        int kernelY = ky + blurLevel;
                        int pixelIndex = (pixelY * width + pixelX) * channels + c;
                        blurredValue += imageData[pixelIndex] * blurKernel[kernelY][kernelX];
                    }
                }
                // Assign the blurred value to the corresponding pixel in the temporary vector
                int index = (y * width + x) * channels + c;
                blurredImage[index] = static_cast<unsigned char>(blurredValue);
            }
        }
    }

    // Copy the blurred image back to the original image data
    imageData = blurredImage;
}

void filter12(string filename) {
    // Load an image from file
    Image image(filename);
    unsigned char *imageData;
    int width, height, channels;
    imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    // Convert imageData to a vector
    vector<unsigned char> imageDataVec(imageData, imageData + width * height * channels);

    // Ask for blur level
    int blurLevel;
    cout << "Enter blur level (0 for no blur): ";
    cin >> blurLevel;

    // Apply blur filter
    applyBlurFilter(imageDataVec, width, height, channels, blurLevel);

    // Save the blurred image
    stbi_write_jpg(filename.c_str(), width, height, channels, imageDataVec.data(), 100);

    cout << "Blur filter applied and saved successfully." << endl;

    // Free memory allocated by stbi_load
    stbi_image_free(imageData);


}

void loadNewImage(string& currentImage) {
    string filename;
    bool validFilename = false;

    while (!validFilename) {
        cout << "Enter the name of the image file (with extension): ";
        cin >> filename;
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "File '" << filename << "' not found. Please enter a valid filename with valid extension.\n";
        } else {
            // Check if the file extension is valid
            string ext = filename.substr(filename.find_last_of(".") + 1);
            if (ext != "jpg" && ext != "bmp" && ext != "png") {
                cout << "Invalid file extension. Please choose a file with '.jpg', '.bmp', or '.png' extension.\n";
            } else {
                validFilename = true;
                currentImage = filename;
            }
        }
    }
    cout << "New image '" << currentImage << "' loaded.\n";
}
void saveImage(string& currentImage) {
    Image image(currentImage);
    if (currentImage.empty()) {
        cout << "No image loaded. Please load an image first.\n";
        return;
    }
    string filename;
    cout << "Enter the name of the image file (with extension): ";
    cin >> filename;
    image.saveImage(filename);

    // Perform the save operation
    // You need to implement the save functionality here
    cout << "Image saved as '" << filename << "'.\n";
}

int main()
{
    string filename,original;
    bool validFilename = false;
    while (!validFilename) {
        cout << "Enter the name of the image file (with extension): ";
        cin >> filename;
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "File '" << filename << "' not found. Please enter a valid filename with valid extention\n." << endl;
        } else {
            validFilename = true;
        }
    }

    Image Temp_image(filename);
    Temp_image.saveImage("CURRENT_VERSION.jpg");
    original = filename;
    filename = "CURRENT_VERSION.jpg";

    while(true)
    {
        cout << "1_Load a new image\n"<<"2_Invert_color\n"<<"3_Rotation\n"<<"4_Blur_image\n"<<"5_Add_frame\n"<<"6_Save_image\n"<<"7_Exit\n";
        cout << "Enter your choice: ";
        char choice;
        cin>>choice;
        if(choice=='1')
        {
            loadNewImage(filename);

            Image Temp_image(filename);
            Temp_image.saveImage("CURRENT_VERSION.jpg");
            original = filename;
            filename = "CURRENT_VERSION.jpg";

            original = filename;
        }
        else if(choice=='2')
        {
            invert_colors(filename);
        }
        else if(choice=='3')
        {
            Rotation1(filename);
        }
        else if(choice=='4')
        {
            filter12(filename);
        }
        else if(choice=='5')
        {
            frame(filename);
        }
        else if(choice=='6')
        {
            saveImage(filename);
        }
        else if(choice=='7')
        {
            cout<<"Thanks for using our programme.";
            break;
        }
        else
        {
            cout<<"Invalid choice try again.\n";
        }
    }
    return 0;
}
