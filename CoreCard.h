/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CardHelper.h
 * Author: krishna
 *
 * Created on June 14, 2019, 1:33 PM
 */

#pragma once

#include <utility>
#include <string>
#include <map>

//#include "QrcodeGenerator.h"

class CoreCard : public Php::Base {
private:
    Php::Value imagecreatetruecolor;
    Php::Value imagecolorallocate;
    Php::Value imagefilledrectangle;
    Php::Value imagecopymerge;
    Php::Value imagecolorat;
    Php::Value imagecolortransparent;
    Php::Value imagesavealpha;
    Php::Value imagealphablending;
    Php::Value imagesx;
    Php::Value imagesy;
    Php::Value imagecopyresized;
    Php::Value imagecolorallocatealpha;
    Php::Value imagecopyresampled;
    Php::Value imagepng;
    Php::Value imagefill;
    Php::Value imagecopy;
    Php::Value imagedestroy;
    Php::Value imageflip;
    Php::Value getimagesize;
    Php::Value imageCreateFromGIF;
    Php::Value imageCreateFromJPEG;
    Php::Value imageCreateFromPNG;
    Php::Value imagefilter;
    Php::Value imageftbbox;
    Php::Value imagefttext;
    Php::Value explode;
    Php::Value backGround;

    std::map<std::string, int>imagetypestoprocess;
    std::string doflip;
    double cardHeight;
    double cardWidth;
    double lineHeight;
    double minTextLine;
    double lineCounter;
    double cardRadious;

    std::string filename;
    std::string cardFont;
    std::string cardFontBold;

public:

    CoreCard() {
        imageCreateFromGIF = "imageCreateFromGIF";
        imageCreateFromJPEG = "imageCreateFromJPEG";
        imageCreateFromPNG = "imageCreateFromPNG";
        imagecreatetruecolor = "imagecreatetruecolor";
        imagecolorallocate = "imagecolorallocate";
        imagefilledrectangle = "imagefilledrectangle";
        imagecopymerge = "imagecopymerge";
        imagecolorat = "imagecolorat";
        imagecolortransparent = "imagecolortransparent";
        imagesavealpha = "imagesavealpha";
        imagealphablending = "imagealphablending";
        imagesx = "imagesx";
        imagesy = "imagesy";
        imagecopyresized = "imagecopyresized";
        imagecreatetruecolor = "imagecreatetruecolor";
        imagecolorallocatealpha = "imagecolorallocatealpha";
        imagecopyresampled = "imagecopyresampled";
        imagepng = "imagepng";
        imagedestroy = "imagedestroy";
        imageflip = "imageflip";
        getimagesize = "getimagesize";
        imagefilter = "imagefilter";
        imagefttext = "imagefttext";
        imagecopy = "imagecopy";
        imagefill = "imagefill";
        imageftbbox = "imageftbbox";
        explode = "explode";

        imagetypestoprocess["image/gif"] = 0;
        imagetypestoprocess["image/jpeg"] = 1;
        imagetypestoprocess["image/png"] = 2;
        //        //variable init
        filename = "noname";
        cardHeight = 500;
        cardWidth = 333.33;
        lineHeight = 4;
        minTextLine = 50;
        lineCounter = 1;
        cardRadious = 5;
        cardFont = "/var/www/html/Tuki/web/resource/organization/card/OpenSans-Regular.ttf";
        cardFontBold = "/var/www/html/Tuki/web/resource/organization/card/open-sans.extrabold.ttf";
        backGround = readImage(cardWidth, cardHeight);
        doflip = "noflip";
    };

    virtual ~CoreCard() {
    };

    //    Php::Value qrcode() {
    //        QrcodeGenerator * qrgen = new QrcodeGenerator();
    //        int result = qrgen->generate('KamalAcharya', "/var/www/html/demoqrcode.bmp");
    //        return result;
    //    }

    void config(Php::Parameters &params) {
        std::map<std::string, std::string>configdata = params[0];
        if (configdata.find("background") != configdata.end()) {
            backGround = createImage(configdata["background"]);
            cardHeight = (double) imagesy(backGround);
            cardWidth = (double) imagesx(backGround);

            Php::Value newImage = imagecreatetruecolor(cardWidth, cardHeight);
            Php::Value alpha_channel = imagecolorallocatealpha(newImage, 0, 0, 0, 127);
            imagecolortransparent(newImage, alpha_channel);
            // Fill image
            imagefill(newImage, 0, 0, alpha_channel);
            // Copy from other
            imagecopy(newImage, backGround, 0, 0, 0, 0, cardWidth, cardHeight);
            // Save transparency
            imagesavealpha(newImage, true);
            // Save PNG
            backGround = newImage;

        }
        if (configdata.find("flip") != configdata.end()) {
            doflip = configdata["flip"];
        }
        if (configdata.find("nfont") != configdata.end()) {
            cardFont = configdata["nfont"];
        }
        if (configdata.find("bfont") != configdata.end()) {
            cardFontBold = configdata["bfont"];
        }
        if (configdata.find("textsize") != configdata.end()) {
            minTextLine = std::stof(configdata["textsize"]);
        }
        if (configdata.find("filename") != configdata.end()) {
            filename = configdata["filename"];
        }
        lineCounter = 1;
    }

    Php::Value setPhoto(Php::Parameters &params) {
        std::string photo = params[0];
        double pox = params[1];
        double poy = params[2];
        double size = params[3];
        bool newsize = params[4];
        return factImage(posX(pox), posY(poy), posX(size), photo, newsize);
    }

    void setGif(Php::Parameters &params) {
        std::string photo = params[0];
        double pox = params[1];
        double poy = params[2];
        double size = params[3];
        addGif(photo, posX(pox), posY(poy), posX(size));
    }

    void setRawText(Php::Parameters &params) {
        std::string stringtext = params[0];
        int font = params[1];
        double pox = params[2];
        double poy = params[3];
        int color = params[4];
        int bold = params[5];
        bool center = false;
        if (params.size() > 6) {
            center = params[6];
        }
        if (center == true) {
            textAtMiddle(stringtext, posX(pox), posY(poy), font, color, bold);
        } else {
            rawtextLabel(stringtext, posX(pox), posY(poy), font, color, bold);
        }
    }

    void setText(Php::Parameters &params) {
        std::string stringtext = params[0];
        int font = params[1];
        double line = params[2];
        bool center = params[3];
        bool newline = params[4];
        if (center == true) {
            textAtMiddle(toUpper(stringtext), posX(cardRadious), lineChange(line, false), font, 1, true);
        } else {
            textLabel(toUpper(stringtext), posX(cardRadious), lineChange(line, false), font, 1, true, newline);
        }
    }

    void setHeader(Php::Parameters &params) {
        std::string text = params[0];
        textAtMiddle(toUpper(text), posX(7), posY(17), 18, 16777215, true);
    }

    Php::Value process() {
        if (doflip == "flip") {
            imageflip(backGround, 1);
        }
        if (filename == "noname") {
            Php::Value returnvalue = imagepng(backGround);
            return returnvalue;
        }
        imagepng(backGround, filename + ".png");
        imagedestroy(backGround);
        return true;
    }

    void addGif(std::string img, double posx, double posy, double hw) {
        Php::Value rawimage = createImage(img);
        imagecolortransparent(rawimage, imagecolorat(rawimage, 0, 0));
        Php::Value width = imagesx(rawimage);
        Php::Value height = imagesy(rawimage);
        Php::Value newheight = height * hw / width;
        Php::Value newImage = imagecreatetruecolor(hw, newheight);
        imagecolortransparent(newImage, imagecolorallocatealpha(newImage, 0, 0, 0, 0));
        imagealphablending(newImage, false);
        imagesavealpha(newImage, true);
        imagecopyresized(newImage, rawimage, 0, 0, 0, 0, hw, newheight, width, height);
        imagecopymerge(backGround, newImage, posx, posy, 0, 0, hw, newheight, 100);
    }

    Php::Value readImage(double w, double h) {
        Php::Value img = imagecreatetruecolor(w, h);
        Php::Value bg = imagecolorallocate(img, 255, 255, 255);
        imagefilledrectangle(img, 0, 0, w, h, bg);
        return img;
    }

    double factImage(double x, double y, double hw, std::string img, bool newsize) {
        Php::Value rawimage = createImage(img);
        imagecolortransparent(rawimage, imagecolorat(rawimage, 0, 0));
        Php::Value width = imagesx(rawimage);
        Php::Value height = imagesy(rawimage);
        double newheight = hw;
        if (newsize == false) {
            newheight = (double) (height * hw / width);
        }
        //        
        Php::Value newImage = imagecreatetruecolor(hw, newheight);
        imagecopyresampled(newImage, rawimage, 0, 0, 0, 0, hw, newheight, width, height);
        imagecopymerge(backGround, newImage, x, y, 0, 0, hw, newheight, 100);
        return (newheight + hw);
    }

    void rawtextLabel(std::string text, double x, double y, int font, int color, bool bold) {
        std::string fontWeight = cardFont;
        if (bold) {
            fontWeight = cardFontBold;
        }
        std::map<std::string, std::string> txtarray = explode(" ", text);
        double pox_x = cardWidth - x;
        std::string textsh = "";
        for (auto &tex : txtarray) {
            std::string tx = tex.second;
            if ((pox_x - getRatio(1) - (tx.length() * getRatio(font))) > 0) {
                textsh += tx + " ";
                pox_x = pox_x - getRatio(1)-(tx.length() * getRatio(font));
            } else {
                textsh += " \n" + tx;
                pox_x = cardWidth - x;
            }
        }
        trim(textsh);
        imagefttext(backGround, getRatio(font), 0, x, y, color, fontWeight, textsh);
    }

    void textLabel(std::string text, double x, double y, int font, int color, bool bold, bool autonewline) {
        std::string fontWeight = cardFont;
        if (bold) {
            fontWeight = cardFontBold;
        }
        std::map<std::string, std::string> txtarray = explode(" ", text);
        double pox_x = cardWidth - x;
        std::string textsh = "";
        for (auto &tex : txtarray) {
            std::string tx = tex.second;
            if ((pox_x - getRatio(1) - (tx.length() * getRatio(font))) > 0) {
                textsh += tx + " ";
                pox_x = pox_x - getRatio(1)-(tx.length() * getRatio(font));
            } else {
                if (autonewline) {
                    lineChange(0.5, false);
                }
                textsh += " \n" + tx;
                pox_x = cardWidth - x;
            }
        }
        if (textsh.find(":") == std::string::npos) {
            trim(textsh);
            imagefttext(backGround, getRatio(font), 0, x, y, color, fontWeight, textsh);
        } else {
            std::map<std::string, std::string> multtext = explode(":", textsh);
            std::string text1 = multtext["0"] + ":";
            Php::Value newImage = imagecreatetruecolor(400, 30);
            int fcolor = imagecolorallocate(newImage, 0, 125, 255);
            trim(textsh);
            trim(text1);
            imagefttext(backGround, getRatio(font), 0, x, y, color, fontWeight, textsh);
            imagefttext(backGround, getRatio(font), 0, x, y, fcolor, fontWeight, text1);
        }
    }

    void textAtMiddle(std::string text, double x, double y, int font, int color, bool bold) {
        std::string fontWeight = cardFont;
        if (bold) {
            fontWeight = cardFontBold;
        }
        std::map<std::string, std::string> txtarray = explode(" ", text);
        double pox_x = cardWidth - x;
        int line = 0;
        std::map<int, std::string> textLink;
        textLink[line] = "";
        for (auto &tex : txtarray) {
            std::string tx = tex.second;
            if ((pox_x - (tx.length() * getRatio(font))) > 0) {
                textLink[line] += tx + " ";
                pox_x = pox_x - (tx.length() * getRatio(font)) - font;
            } else {
                line++;
                textLink[line] = tx + " ";
                pox_x = cardWidth - x - (tx.length() * getRatio(font)) - font;
            }
        }

        for (auto &ttx : textLink) {
            std::string tx = ttx.second;
            trim(tx);
            imagefttext(backGround, getRatio(font), 0, stringCenter(tx, x, font), y, color, fontWeight, tx);
            y = y + posY(lineHeight);
        }
    }

    Php::Value createImage(std::string imagename) {
        Php::Value image = nullptr;
        Php::Value info = getimagesize(imagename);
        std::string switchvalue = info["mime"];
        switch (imagetypestoprocess[switchvalue]) {
            case 0:
                image = imageCreateFromGIF(imagename);
                break;
            case 1:
                image = imageCreateFromJPEG(imagename);
                break;
            case 2:
                image = imageCreateFromPNG(imagename);
                break;
        }
        return image;
    }

    double posX(double x) {
        return (x * cardWidth) / 100;
    }

    double posY(double y) {
        return (y * cardHeight) / 100;
    }

    double getRatio(double r) {
        return r * 1.4;
    }

    double stringCenter(std::string strg, int x, int font) {
        std::map<std::string, double>list = imageftbbox(getRatio(font), 0, cardFontBold, strg);
        double posx = (cardWidth - list["2"] - list["0"]) / 2;
        return posx;
    }

    double lineChange(double line, bool post) {
        if (post) {
            lineCounter += line;
            return lineCounter * posY(lineHeight) + posY(minTextLine);
        } else {
            double out = lineCounter * posY(lineHeight) + posY(minTextLine);
            lineCounter += line;
            return out;
        }
    }

    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    std::string toUpper(std::string textstring) {
        for (int i = 0; i < textstring.length(); i++) {
            textstring[i] = std::toupper(textstring[i]);
        }
        return textstring;
    }
};


