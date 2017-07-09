#include <Arduino.h>
#include <avr/pgmspace.h>

#define FIRST 0,1
#define LAST m.length()-1
#define debug(X) Serial.print(F("debug: ")); Serial.println(F(X));

const char cmdStart = '#';
const char cmdEnd = ';';
const char pathStart = '{';
const char pathEnd = '}';
const char separator = ',';
const char separatorReplace = 135; // ASCII ‡


class Command {
    private:
        class Params_t {
            private:
                String xParams;
                int numberOfParams = 0;

            public:
                // TODO: Pokud by byla na vstupu čárka, nahradit separatorReplace znakem
                int length() {
                    return numberOfParams;
                }

                String toString() {
                    return xParams.substring(0, xParams.length() - 1);
                }

                void set(const char *p) {
                    xParams = p;
                    xParams.concat(separator);
                    for (auto i = 0; i < (signed)xParams.length(); i++)
                        if (xParams[i] == separator)
                            numberOfParams++;
                }

                void set(String &p) {
                    xParams = p;
                    xParams.concat(separator);
                    for (auto i = 0; i < (signed)xParams.length(); i++)
                        if (xParams[i] == separator)
                            numberOfParams++;
                }


                void pop() {
                    xParams.remove(xParams.length() - 1); // remove last separator

                    while (xParams[xParams.length() - 1] != separator)
                        xParams.remove(xParams.length() - 1);

                    numberOfParams--;
                }

                void append(const char *param) {
                    xParams += String(param) + separator;
                    numberOfParams++;
                }

                void append(String &param) {
                    xParams += String(param) + separator;
                    numberOfParams++;
                }

                String get(int nth) {
                    int lastnth = 0;
                    nth++;

                    if (nth > numberOfParams) {
                        debug("getParam: wrong index");
                        return "";
                    }

                    for (auto i = 0; i < nth; i++)
                        lastnth = xParams.indexOf(separator, lastnth + 1);

                    if (nth == 1)
                        return xParams.substring(0, lastnth);

                    return xParams.substring(xParams.lastIndexOf(separator, lastnth - 1) + 1, lastnth);
                }

        };

        class Header_t {
            private:
                String xHeader;

            public:
                // Header
                void set(const char *h) {
                    xHeader = h;
                }

                void set(String &h) {
                    xHeader = h;
                }

                String& get() {
                    return xHeader;
                }

                bool is(const String h) {
                    return xHeader == h;
                }

                // operator String() {
                //     return xHeader;
                // }

        };

        class Path_t {
            private:
                String xPath;

            public:
                // Path, NOTE: getNextPointName & getNextPointAttr pracují s prvním pointem, tzn. point musí být unikátní!!!!
                void set(const char *p) {
                    xPath = p;
                }

                void set(String &p) {
                    xPath = p;
                }

                String& get() {
                    return xPath;
                }

                char getNextPointName(char basePoint) {
                    int index = xPath.indexOf(basePoint);

                    if (index == -1)
                        return -1;

                    while (isdigit(xPath[index + 1]))
                        index++;

                    if (!isalpha(xPath[index + 1]))
                        return -1;

                    return xPath[index + 1];
                }

                int getPointAttr(char basePoint) {
                    int index = xPath.indexOf(basePoint);
                    String strnum = "";

                    if (index == -1)
                        return -1;

                    if (!isdigit(xPath[index + 1]))
                        return -1;

                    for (; isdigit(xPath[index + 1]); index++)
                        strnum += xPath[index + 1];

                    return strnum.toInt();
                }

                int getNextPointAttr(char basePoint) {
                    return getPointAttr(getNextPointName(basePoint));
                }

                bool addPoint(char name, signed int attribute) {
                    if (isalpha(name)) {
                        xPath.concat(name);
                        xPath.concat(attribute);
                        return true;
                    }
                    else return false;
                }

                void popPoint() {
                    if (xPath.length() == 0)
                        return;

                    while (isdigit(xPath[xPath.length() - 1]))
                        xPath.remove(xPath.length() - 1);  // Remove point attribute

                    xPath.remove(xPath.length() - 1); // Remove point name
                }

        };


    public:
        Params_t params;
        Header_t header;
        Path_t path;

        Command () {}

        Command(const char *m) {
            String s = String(m);
            decompose(s);
        }

        Command(char *&m) {
            String s = String(m);
            decompose(s);
        }

        Command(String &m) {
            decompose(m);
        }

        Command(String m) {
            decompose(m);
        }


        // Command getInstance() {
        //     return *this;
        // }


        // Implicit conversion to String, you don't have to use String x = cmd.compose(), just use String x = cmd;
        operator String() {
            return compose();
        }


        // BUG: String& compose() - Reference - části textu byly zvláštní.
        String compose() {
            return String(cmdStart) +
                   header.get() +
                   String(pathStart) +
                   path.get() +
                   String(pathEnd) +
                   params.toString() +
                   String(cmdEnd);
        }

        void decompose(String &m) {
            m.trim();
            if (m[0] != cmdStart || m[m.length() - 1] != cmdEnd) {
                debug("[FAIL] first is not '#' or last is not ';'");
                return;
            }

            m.remove(FIRST);
            m.remove(LAST);
            m += ","; // :)

            while (m[0] != pathStart) {
                header.set(header.get() + String(m[0]));
                // xHeader += String(m[0]);
                m.remove(FIRST);
            }

            m.remove(FIRST);

            while (m[0] != pathEnd) {
                path.set(path.get() + String(m[0]));
                // xPath += String(m[0]);
                m.remove(FIRST);
            }

            m.remove(FIRST);

            params.set(m);
        }


};
