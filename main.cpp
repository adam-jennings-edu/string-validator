#include <iostream>
#include <string>
#include <set>
#include <fstream>

using namespace std;

// Validator class contains the logic and structures required for string input validation based on minimum length requirements, and different character filtering modes
//

class Validator{

    public :
        enum FilterType {BLACKLIST, WHITELIST, NONE}; // These are the different modes of input filtering available, each type has a corresponding filter class which implements the filter logic.

        // Data type for returning a boolean response and message to report reasons for validation failures
        struct ValidationResponse  {
            public :
                bool responseValue = true;
                string responseMessage = "";
        };

        // constructor for Validator
        // set minimum character length (I wasn't sure what the right way to pass in a varaible with the same name as a member variable, so it got the underscore treatment)
        // instantiate correct filter object type based on filter mode, and pass pointer to filterset to that filter object. (if applicable)
        Validator(int _minLength = 0, FilterType filterMode = NONE, set<char>* filterSet = NULL){
            minLength = _minLength;
            switch (filterMode){                            // Switch just checks which mode was set and instantiates the appropriate type of filter object
                case BLACKLIST:
                    filter = new BlacklistFilter(filterSet);
                    break;
                case WHITELIST:
                    filter = new WhitelistFilter(filterSet);
                    break;
                default:
                    filter = new Filter();
                    break;
            }
        }

        ValidationResponse isInputValid(string input){
            // I chose to keep a ValidationResponse which I populate and return at the end of the function instead of returning immediately
            // on failure to allow for the responseMessage in the ValidationResponse to include multiple causes of validation failure
            // that way if a string fails validation for being too short, and the letters fail the filter check as well, it reports
            // both of those causes and not just the first one checked. There's definitely room for some improvement in this system, but it works for a demo
            ValidationResponse validationResponse;

            //check that input is of minimum length
            if (hasMinimumLength() && input.length() < minLength){
                //if input fails this check, set response to false and add message.
                validationResponse.responseValue = false;
                validationResponse.responseMessage.append("Does not meet minimum length. (" + to_string(input.length()) + " / (" + to_string(minLength) + ")\n");
            }

            //
            ValidationResponse filterResponse = filter->check(input);   // perform the filter check
            validationResponse.responseValue = validationResponse.responseValue && filterResponse.responseValue;    // AND the responseValue with the responsvalue of the filter so it doesn't unfalsifiy a result from minimum length
            validationResponse.responseMessage.append(filterResponse.responseMessage);  // add message from filter check response

            // validationResponse has everything we need, send it back home.
            return validationResponse;
        }

        private:
            // Base class of filter objects.
            // These objects implement a check function to compare the input against that filter's process of validating input.
            class Filter{
                public:
                    Filter(){}
                    virtual ValidationResponse check(string input){
                        cout << "Base Check" << endl;
                        ValidationResponse validationResponse;
                        return validationResponse;                  // this feels like it could be done in one line?
                    }

                protected :
                    set<char> filterSet;
            };

            // Blacklist filter class
            // to pass check function, none of the characters in input can be contained in the filterSet
            class BlacklistFilter : public Filter{
                public:
                    BlacklistFilter(set<char>* initFilterSet){
                        if (initFilterSet != NULL)                  // I'm not completely clear on why, but if I don't perform this check, it will crash at runtime when this value is null
                            filterSet = *initFilterSet;
                    }
                    ValidationResponse check(string input){
                        ValidationResponse validationResponse;
                        for (int i = 0; i < input.size(); i++){     // walk through each character of the input string
                            char currentChar = input[i];            // cache the current character (so we don't have to keep calling it input[i])
                            for (char listChar : filterSet){        // walk through each character in the filterSet set
                                if (currentChar == listChar){       // Validation fails if the string contains a blacklisted character
                                    validationResponse.responseMessage.append("Input contains blacklisted character(s) \n");
                                    validationResponse.responseValue = false;
                                    return validationResponse;
                                }
                            }
                        }
                        return validationResponse;
                    }
            };

            // Whitelist filter class
            // to pass check function, all of the characters in input must be contained in the filterSet
            class WhitelistFilter : public Filter {
                public:
                    WhitelistFilter(set<char>* initFilterSet){
                        if (initFilterSet != NULL)                  // I'm not completely clear on why, but if I don't perform this check, it will crash at runtime when this value is null
                            filterSet = *initFilterSet;
                    }
                    ValidationResponse check(string input){
                        ValidationResponse validationResponse;
                        for (int i = 0; i < input.size(); i++){     // walk through each character of the input string
                            char currentChar = input[i];            // cache the current character (so we don't have to keep calling it input[i])

                            bool foundInWhitelist = false;          // track if we have found the character in the filterSet or not
                                for (char listChar : filterSet){    // walk through each character in the filterSet
                                    if (currentChar == listChar){
                                        foundInWhitelist = true;    // character was found in filterList
                                        break;                      // no need to check any other white list entries, we already foudn a match
                                    }
                                }
                                if (!foundInWhitelist){             // Validation fails if the character does not match any of the whitelisted characters
                                    validationResponse.responseMessage.append("Input contains character(s) outside of whitelist \n");
                                    validationResponse.responseValue = false;
                                    return validationResponse;      // no need to check any more characters in the string, it already failed validation
                                }
                        }
                        return validationResponse;
                    }
            };

            int minLength; // minimum length to require of strings (0 or below = no minimum)
            Filter* filter; // Filter object to perform character filtering

            // little helper function to make it more clear what the purpose of comparing minLength and 0 is
            bool hasMinimumLength(){
                return minLength > 0;
            }
};

int main ()
{

    cout << "Welcome to the Adam Jennings Computer Science understanding example program!" << endl;
    cout << "This program was written to demonstrate an understanding of my Computer Science principles." << endl;
    cout << "This was requested to ensure that I'm set up for success as well as to refamiliarize myself with some of c++'s quirks. Hopefully it serves those purposes!" << endl << endl;

    cout << "This program gives the user options to choose some criteria with which to determine if a string is considered to be \"valid\" or not" << endl;
    cout << "The options presented to the user are :" << endl;
    cout << "\t-The user can choose to require that a string be of a minimum length" << endl;
    cout << "\t-The user can choose to allow a list of characters to serve as a blacklist (a string is not valid if it contains any black-listed characters)" << endl;
    cout << "\t-The user can choose to allow a list of characters to serve as a whitelist (a string is only valid if it is composed entirely of white-listed characters)" << endl;
    cout << "for the whitelist/blacklist, I have provided some preset lists for the user to choose from, or they may create a custom list" << endl << endl;

    cout << "once the criteria is set, the program will apply it to a list of strings stored in \"strings.txt\"" << endl;
    cout << "It will then provide details about how many strings met / did not meet the criteria" << endl << endl;

    int minChars = 0;
    Validator::FilterType filterType = Validator::FilterType::NONE;
    set<char> filterSet;

    // I decided to not get too fancy with the user input, the bulk of what's interesting is in the actual validation process.
    string userInput;

    //minimum character selection
    do {
        cout << "Would you like to set a minimum character requirement? (Y or N) : ";
        cin >> userInput;
    }
    while (userInput != "Y" && userInput != "y" &&
           userInput != "N" && userInput != "n");

    if (userInput == "Y" || userInput == "y"){
        bool isInputNumeric = false;
        do {
            isInputNumeric = true;
            cout << "Minimum character requirement (Numbers only) : ";
            cin >> userInput;
            for (char character: userInput){
                if ( character < 48 || character > 57){isInputNumeric = false;}
            }
        }
        while(!isInputNumeric);
        minChars = stoi(userInput);
    }
    // Filter type selection
    do {
        cout << "select filter type (B)lacklist, (W)hitelist, (N)one  : ";
        cin >> userInput;
    }
    while (userInput != "B" && userInput != "b" &&
           userInput != "W" && userInput != "w" &&
           userInput != "N" && userInput != "n");
    if (userInput == "B" || userInput == "b"){
        filterType = Validator::FilterType::BLACKLIST;
    }
    if (userInput == "W" || userInput == "w"){
        filterType = Validator::FilterType::WHITELIST;
    }
    // Filter list selection
    if (filterType != Validator::FilterType::NONE){
        do {
            cout << "select filter list (L)etters, (A)lphanumeric, Alphanumeric w/ (S)pecial characters, (C)ustom : ";
            cin >> userInput;
        }
        while (userInput != "L" && userInput != "l" &&
               userInput != "A" && userInput != "a" &&
               userInput != "S" && userInput != "s" &&
               userInput != "C" && userInput != "c");

        //populate filterSet from presets
        //if the user did not choose custom list, populate from presets
        if (userInput!= "C" && userInput!= "c"){
            //all options get letters
            for (int i = 0; i < 26; i++){
                filterSet.insert((char)(i + 65));
                filterSet.insert((char)(i + 97));
            }

            // either option that is not just letters, gets numbers as well
            if (userInput!= "L" && userInput!= "l"){
                for (int i = 0; i <= 9; i++){
                    filterSet.insert((char)(i + 48));
                }
            }
            // only Special characters option gets special characters
            if (userInput == "S" || userInput == "s"){
                for (char character : "!@#$%^&*()"){
                    filterSet.insert(character);
                }
            }
        }
        //Custom option was chosen
        else{
            int setLength;
            bool isInputNumeric = false;
            do {
                isInputNumeric = true;
                cout << "Enter number of characters to add to list (Numbers only) : ";
                cin >> userInput;
                for (char character: userInput){
                    if ( character < 48 || character > 57){isInputNumeric = false;}
                }
            }
            while(!isInputNumeric);
            setLength = stoi(userInput);

            while(filterSet.size() < setLength) {
                char charInput;
                if (filterSet.size() > 0){
                    cout << "Filter set currently contains : ";
                    for (char entry : filterSet){
                        cout << "\'" << entry << "\', ";
                    }
                    cout << endl;
                }

                cout << "Enter a character to add to the Filter list (" << (setLength - filterSet.size()) << " remaining)";
                cin >> charInput;
                filterSet.insert(charInput);
            }
        }
    }


    //create validator
    Validator validator = Validator(minChars,filterType,&filterSet);


    // run validation check on each line in file

    int passCount = 0;
    int failCount = 0;

    const string dataFileName = "strings.txt";
    ifstream dataFile;
    dataFile.open(dataFileName);

    if (dataFile.is_open()){
        string lineText;
        while ( getline (dataFile,lineText) )
        {
          Validator::ValidationResponse response = validator.isInputValid(lineText);
          if (response.responseValue == true){
            passCount ++;
          }
          else{
            failCount ++;
          }
        }
        dataFile.close();
    }
    else{
        cout << "There was a problem opening the strings file. Ensure that " << dataFileName << " exists in this directory";
        return 1;
    }

    cout << endl;
    cout << "Passed String count : " << passCount << endl;
    cout << "Failed String count : " << failCount << endl;

    return 0;
}
