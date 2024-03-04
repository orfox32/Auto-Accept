#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

using namespace std;
using json = nlohmann::json;

struct MemoryStruct
{
    char *memory;
    size_t size;
};

void saveUserDirectory(const string &directory)
{
    json config;
    config["directory"] = directory;
    
    ofstream configFile("config.json");
    if (configFile.is_open())
    {
        configFile << config.dump(4); 
        configFile.close();
        cout << "User directory saved successfully." << endl;
    }
    else
    {
        cerr << "Unable to save user directory." << endl;
    }
}

string readUserDirectory()
{
    ifstream configFile("config.json");
    if (configFile.is_open())
    {
        json config;
        configFile >> config;
        configFile.close();
        if (config.contains("directory"))
            return config["directory"];
    }
    return "";
}

string getUserDirectory()
{
    string directory;
    cout << "Enter the directory where League of Legends is installed: ";
    getline(cin, directory);
    return directory;
}

string findLockFile(const string &gamedir)
{
    string lockfilePath = gamedir + "/lockfile";
    ifstream lockfile(lockfilePath);
    if (lockfile.good())
    {
        cout << "Found running League of Legends in directory: " << gamedir << endl;
        return lockfilePath;
    }
    return "";
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *buffer)
{
    size_t totalSize = size * nmemb;
    buffer->append((char *)contents, totalSize);
    return totalSize;
}

std::string base64_encode(const std::string &input)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    return std::string(bufferPtr->data, bufferPtr->length);
}

bool sendHttpRequest(const string &url, const string &authorization, string &response, const string &requestType)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl)
    {
        cerr << "Failed to initialize CURL." << endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (requestType == "POST")
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, authorization.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    if (requestType == "POST")
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    }

    res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "Error: " << curl_easy_strerror(res) << endl;
        return false;
    }

    return true;
}

bool checkLoggedIn(const string &port, const string &authorization)
{
    string url = "https://127.0.0.1:" + port + "/lol-login/v1/session";
    string response;

    return sendHttpRequest(url, authorization, response, "GET");
}

bool isLockFileExists(const string &lockfilePath)
{
    struct stat buffer;
    return (stat(lockfilePath.c_str(), &buffer) == 0);
}

int main()
{
    string userDirectory = readUserDirectory();

    string choice;
    if (!userDirectory.empty())
    {
        cout << "League of Legends directory found: " << userDirectory << endl;
        cout << "Do you want to use this directory? (yes/no): ";
        getline(cin, choice);
    }

    if (userDirectory.empty() || choice == "no")
    {
        userDirectory = getUserDirectory();
        saveUserDirectory(userDirectory);
    }


    string lockfilePath;
    int attempts = 0;
    while (lockfilePath.empty() && attempts < 10)
    {
        lockfilePath = findLockFile(userDirectory);
        if (lockfilePath.empty())
        {
            attempts++;
            cout << "Unable to find Lockfile. Attempt: " << attempts << "\r" << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    if (lockfilePath.empty())
    {
        cerr << "Error: Unable to find Lockfile." << endl;
        return 1;
    }

    ifstream lockFileStream(lockfilePath);
    if (!lockFileStream.is_open())
    {
        cerr << "Error: Unable to open lock file." << endl;
        return 1;
    }

    string lockdata;
    getline(lockFileStream, lockdata);
    lockFileStream.close();

    vector<string> lockDataParts;
    size_t pos = 0;
    while ((pos = lockdata.find(':')) != string::npos)
    {
        lockDataParts.push_back(lockdata.substr(0, pos));
        lockdata.erase(0, pos + 1);
    }
    lockDataParts.push_back(lockdata);

    if (lockDataParts.size() != 5)
    {
        cerr << "Error: Invalid lock data format." << endl;
        return 1;
    }

    string procname = lockDataParts[0];
    string pid = lockDataParts[1];
    string port = lockDataParts[2];
    string password = lockDataParts[3];
    string protocol = lockDataParts[4];

    string username = "riot";
    string userpass = username + ":" + password;
    string encodedUserpass = base64_encode(userpass);
    string authorization = "Authorization: Basic " + encodedUserpass;

    string summonerId;
    bool loggedIn = false;
    while (!loggedIn)
    {
        cout << "Checking login status..." << endl;
        try
        {

            if (!isLockFileExists(lockfilePath))
            {
                cerr << "Error: League of Legends client is not running." << endl;
                return 1;
            }

            if (!checkLoggedIn(port, authorization))
            {
                cout << "Waiting for login..." << endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            cout << "Login successful!" << endl;
            loggedIn = true;

            string phase;
            while (true)
            {
                string phaseUrl = "https://127.0.0.1:" + port + "/lol-gameflow/v1/gameflow-phase";
                string phaseResponse;
                if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET"))
                {
                    try
                    {
                        json phaseJson = json::parse(phaseResponse);
                        phase = phaseJson.get<string>();
                        cout << "Current phase: " << phase << "\r" << std::flush;

                        if (phase == "ReadyCheck")
                        {
                            string acceptUrl = "https://127.0.0.1:" + port + "/lol-matchmaking/v1/ready-check/accept";
                            string acceptResponse;
                            if (!sendHttpRequest(acceptUrl, authorization, acceptResponse, "POST"))
                            {
                                cerr << "Failed to auto-accept match." << endl;
                            }
                        }
                        else if (phase == "ChampSelect")
                        {
                            cout << "ChampSelect phase detected. Waiting..." << "\r" << std::flush;
                        }
                        else if (phase == "InProgress")
                        {
                            cout << "Game is in progress. Exiting." << endl;
                            std::this_thread::sleep_for(std::chrono::seconds(10));
                            return 0;
                        }
                    }
                    catch (json::exception &e)
                    {
                        cerr << "Error parsing JSON response: " << e.what() << endl;
                    }
                }
                else
                {
                    cerr << "Failed to retrieve gameflow phase." << endl;
                }

                std::this_thread::sleep_for(std::chrono::seconds(1));

                if (!isLockFileExists(lockfilePath))
                {
                    cerr << "League of Legends client has been closed." << endl;
                    return 1;
                }
            }
        }
        catch (const std::exception &e)
        {
            cerr << "An error occurred: " << e.what() << endl;
        }
    }

    return 0;
}
