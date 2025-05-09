#include "dataHandler.hpp"

#include "../backend/cpp/run.hpp"

static std::vector<std::vector<std::string>> temp;

int generateData(int length, std::string Type) {
    std::vector<std::string> headerStr; // Vector of headers
    std::vector<std::string> dataStr;   // Vector of data
    std::vector<int> indices(length);   // Vector to hold indices 0 to length-1
    std::iota(indices.begin(), indices.end(), 0); // Fill with 0, 1, 2, ..., length-1

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Shuffle the indices randomly
    std::shuffle(indices.begin(), indices.end(), gen);

    std::stringstream lengthStr;
    lengthStr << length;

    headerStr.push_back("Length"); // Add "Length" as a string
    dataStr.push_back(lengthStr.str());

    for (int i = 0; i < length; i++) { // Iterate from 0 to length - 1 directly
        std::stringstream arrayIdx;
        std::stringstream arrayInt;
        arrayIdx << "A" << i;
        arrayInt << indices[i] + 1;
        headerStr.push_back(arrayIdx.str());
        dataStr.push_back(arrayInt.str());
    }

    headerStr.push_back("Type"); // Add "Type" as a string
    dataStr.push_back(Type);

    temp.push_back(headerStr);
    temp.push_back(dataStr);

    // open file to write to
    std::ofstream outputFile("../../data/programIn.csv");

    if (!outputFile.is_open())
    {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return 1;
    }

    // Check for first index of array (length)
    // Note: The element at temp[0][1] will now correspond to the *first* randomized index.
    // If you intended to always access the *first* generated array element, regardless of randomization,
    // you might need to adjust how you access it.
    if (!temp[0].empty() && temp[0].size() > 1) {
        std::string secondRowFirstColumn = temp[0][1];
        std::cout << "0, 1: " << secondRowFirstColumn << std::endl;
    } else {
        std::cerr << "Error: temp[0] does not have at least two elements." << std::endl;
    }

    // write the temp vector to the file
    for (const auto& row : temp)
    { // Iterate through each row in temp
        for (size_t i = 0; i < row.size(); ++i)
        { // Iterate through each element in the row
            outputFile << row[i];
            if (i < row.size() - 1)
            {
                outputFile << ","; // Add comma separator
            }
        }
        outputFile << std::endl; // Add newline for the next row
    }

    outputFile.close();
    return 0;
}

int pullDataFromFunctions() {
    int pythonResult = system("python ../../src/backend/python/parseData.py ../../data/programIn.csv ../../data/pythonOut.csv");
    if (pythonResult == 0) {
        std::cout << "Python script executed successfully." << std::endl;
    } else {
        std::cerr << "Error executing Python script." << std::endl;
        return 1;
    }

    int cppResult = runBack();
    if (cppResult == 0) {
        std::cout << "C++ script executed successfully." << std::endl;
    } else {
        std::cerr << "Error executing C++ script." << std::endl;
        return 1;
    }
    return 0;
}

int displayDataToScreen(SDL_Renderer* renderer, SDL_Window* window, SDL_Surface* screenSurface, int x, int y, int w, int h, int bordorWidth) {
    /**
     * Here needs to parse the data and then display it
     * Maybe we have an input box somewhere on the screen which takes input and calls
     * GenerateData() with the arguments
     * I have something written in c but that doesn't transfer over...
     * imma let you deal with this lol
     */

    // This will all need to be reworked (probably using classes) for it to be cleanly done
    
    /**
     * from here, plan is to get bounds of full window and allow for buttons on the bottom, 
     * as well as 2 different surfaces which contain the sorts
     */
    
    int marginX = 25;
    int marginY = 250;
    int width = screenSurface->w;
    int height = screenSurface->h;

    // !!! FIXME !!! - Not sure why but need to multiply marginY * 2 for height... i could be wrong
    // but i dont think thats correct
    SDL_Surface* cppSortSurface = SDL_CreateSurface((width / 2) - (marginX * 2), height - (marginY * 2), screenSurface->format);
    SDL_Surface* pythonSortSurface = SDL_CreateSurface((width / 2) - (marginX * 2), height - (marginY * 2), screenSurface->format);

    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(screenSurface->format);
    Uint32 red = SDL_MapRGBA(details, NULL, 255, 0, 0, 255);

    SDL_Rect rect = { x, y, w, h };
    SDL_FillSurfaceRect(cppSortSurface, &rect, red);
    SDL_FillSurfaceRect(pythonSortSurface, &rect, red);

    SDL_Texture* cppTexture = SDL_CreateTextureFromSurface(renderer, cppSortSurface);
    SDL_Texture* pythonTexture = SDL_CreateTextureFromSurface(renderer, pythonSortSurface);
	if (!cppTexture || !pythonTexture) {
		SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
		return 1;
	}

    // Handle organizing of the location of the rects
    SDL_FRect cppDst = { (float)marginX, (float)marginY, (float)cppSortSurface->w, (float)cppSortSurface->h };
    SDL_FRect pythonDst = { (float)marginX + (width / 2), (float)marginY, (float)pythonSortSurface->w, (float)pythonSortSurface->h };

    SDL_FRect cppBorderRect = { (float)(marginX - bordorWidth), (float)(marginY - bordorWidth), (float)((width / 2) - (marginX * 2) + (bordorWidth * 2)),  (float)(height - (marginY * 2)  + (bordorWidth * 2))};
    SDL_FRect pythonBorderRect = { (float)(marginX + (width / 2) - bordorWidth), (float)(marginY - bordorWidth), (float)((width / 2) - (marginX * 2) + (bordorWidth * 2)),  (float)(height - (marginY * 2)  + (bordorWidth * 2))};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &cppBorderRect);
    SDL_RenderFillRect(renderer, &pythonBorderRect);

	SDL_RenderTexture(renderer, cppTexture, nullptr, &cppDst);
    SDL_RenderTexture(renderer, pythonTexture, nullptr, &pythonDst);

	SDL_DestroyTexture(cppTexture);
	SDL_DestroySurface(cppSortSurface);

    SDL_DestroyTexture(pythonTexture);
	SDL_DestroySurface(pythonSortSurface);

    return 0;
}