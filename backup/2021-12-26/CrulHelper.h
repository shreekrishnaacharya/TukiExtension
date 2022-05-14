/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CrulHelper.h
 * Author: krishna
 *
 * Created on March 22, 2019, 6:10 PM
 */

#include "curl/include/curl/curl.h"
#include <sstream>

class CurlHelper {
public:
    std::string responsecode = "200";
    std::string responsemessage = "OK";
    bool progress = true;

    std::string request(std::string url) {
        curl_global_init(CURL_GLOBAL_ALL);
        std::string html = "Unable to connect to server.";
        std::ostringstream oss;
        if (CURLE_OK == curl_read(url, oss)) {
            // Web page successfully written to string
            html = oss.str();
        } else {
            responsecode = "500";
            responsemessage = "ERROR OCCURED";
        }
        curl_global_cleanup();
        return html;
    }
    
    std::string request_post(std::string url, std::string post) {
        curl_global_init(CURL_GLOBAL_ALL);
        std::string html = "Unable to connect to server.";
        std::ostringstream oss;
        if (CURLE_OK == curl_read_post(url, oss, post)) {
            // Web page successfully written to string
            html = oss.str();
        } else {
            responsecode = "500";
            responsemessage = "ERROR OCCURED";
        }
        curl_global_cleanup();
        return html;
    }

    std::string download(std::string url, char outfilename[FILENAME_MAX], std::string post = "", long timeout = 2400) {
        curl_global_init(CURL_GLOBAL_ALL);
        std::FILE *fp;
        fp = fopen(outfilename, "wb");
        std::string html = "FAILED";
        CURLcode code(CURLE_FAILED_INIT);
        CURL* curl = curl_easy_init();
        bool pflag = true;
        if (curl) {
            if (progress) {
                if (CURLE_OK != (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L))) {
                    pflag = false;
                }
            }
            if (pflag && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post))) {
                code = curl_easy_perform(curl);
            }
            curl_easy_cleanup(curl);
        }
        if (CURLE_OK == code) {
            html = "SUCCESS";
        } else {
            responsecode = "500";
            responsemessage = "ERROR OCCURED";
        }
        fclose(fp);
        curl_global_cleanup();
        return html;
    }

private:
    // callback function writes data to a std::ostream

    size_t write_file(void *ptr, size_t size, size_t nmemb, FILE *stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp) {
        if (userp) {
            std::ostream& os = *static_cast<std::ostream*> (userp);
            std::streamsize len = size * nmemb;
            if (os.write(static_cast<char*> (buf), len))
                return len;
        }

        return 0;
    }

    /**
     * timeout is in seconds
     **/
    CURLcode curl_read(const std::string& url, std::ostream& os, long timeout = 30) {
        CURLcode code(CURLE_FAILED_INIT);
        CURL* curl = curl_easy_init();
        if (curl) {
            if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
                code = curl_easy_perform(curl);
            }
            curl_easy_cleanup(curl);
        }
        return code;
    }
    
    CURLcode curl_read_post(const std::string& url, std::ostream& os, std::string post, long timeout = 30) {
        CURLcode code(CURLE_FAILED_INIT);
        CURL* curl = curl_easy_init();
        if (curl) {
            if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))
                    && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str()))) {
                code = curl_easy_perform(curl);
            }
            curl_easy_cleanup(curl);
        }
        return code;
    }
};