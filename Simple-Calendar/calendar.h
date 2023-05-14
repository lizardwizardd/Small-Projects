#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <utility>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
#include <fstream>

#include "console.cpp"

#define MAX_EVENT_DESC 60
#define MAX_EVENT_NAME 50


class Date
{
    private:
        int year;    // min value 1801
        int month;   // starts with month 1
        int day;     // starts with day 1

    public:
        Date(int y, int m, int d) : year(y), month(m), day(d) {}
        Date() { year = 0; month = 0; day = 0; }

        bool operator<(const Date otherDate) const
        {
            if (year != otherDate.year) return year < otherDate.year;
            if (month != otherDate.month) return month < otherDate.month;
            return day < otherDate.day;
        } 

        bool equalDate(Date date)
        {
            if (day == date.day && month == date.month && year == date.year)
                return true;
            else
                return false;
        }

        bool compareMonthYear(Date date)
        {
            if (month == date.month && year == date.year)
                return true;
            else
                return false;
        }

        int getYear() const { return year; }
        int getMonth() const { return month; }
        int getDay() const { return day; }

        void setDay(int x) 
        {
            if (x < 1 || x > 31) // day must be in range of 1 - 31
            {
                throw std::invalid_argument("Invalid value for day\n");
            }
            
            if (this->month == 2)  // check the amount of days for Feb
            {
                bool isLeapYear = ((this->year % 4 == 0 && this->year % 100 != 0) || (this->year % 400 == 0));
                if (isLeapYear && x > 29)
                {
                    throw std::invalid_argument("There's only 29 days in leap year in February\n");
                }
                else if (!isLeapYear && x > 28)
                {
                    throw std::invalid_argument("There's only 28 days in non-leap year in February\n");
                }
            }
            else if ((this->month == 4 || this->month == 6 || this->month == 9 || this->month == 11) && x > 30)
            {
                // for Apr, Jun, Sep and Nov there's not more than 30 days
                throw std::invalid_argument("There's only 30 days in this month\n");
            }
            
            this->day = x;
        }
        void setMonth(int m)
        {
            if (m < 1 || m > 12) // month must be in range of 1 - 12
            {
                throw std::invalid_argument("Invalid value for month\n");
            }
            
            this->month = m;
        }
        void setYear(int y)
        {
            if (y <= 1800) // year must be greater than 1800
            {
                throw std::invalid_argument("Invalid value for year\n");
            }
            
            this->year = y;
        }

        void inputDateFromConsole() 
        {
            int day, month, year;
            std::cin >> day >> month >> year;
            try 
            {
                setDay(day);
                setMonth(month);
                setYear(year);
            }
            catch (const std::invalid_argument& e) {
                throw e;
            }
        }

        void printDate()
        {
            std::cout << day << "." << month << "." << year;
        }
};

class Event
{
    private:
        std::string name;
        std::string description;
        Date currentDate;
    public:
        Event() {};
        Event(std::string name, std::string description, Date date) : name(name), description(description), currentDate{date}
        {
            this->name = name;
            this->description = description;
            this->currentDate = date;
        }
        Date getDate() const { return currentDate; }
        std::string getName() const { return name; }
        std::string getDescription() const { return description; }

        void changeDescription(std::string newDescription)
        {
            if (newDescription.size() > MAX_EVENT_DESC)
            {
                throw std::invalid_argument("Description can't be longer than 60 symbols\n");
            }
            description = newDescription;
        }

        void changeName(std::string newName)
        { 
            if (newName.size() > MAX_EVENT_NAME)
            {
                throw std::invalid_argument("Name can't be longer than 60 symbols\n");
            }
            this -> name = newName;
        }

        void changeDate(Date newDate)
        {
            currentDate = newDate;
        }

        void displayBasicInfo() const
        {
            std::cout << "Name: " << name << '\n';
            std::cout << "Date: " << std::setw(2) << std::setfill('0') << currentDate.getDay() << '.' << std::setw(2) << std::setfill('0') << currentDate.getMonth() << '.' << currentDate.getYear() << '\n';
            std::cout << "\n";
        }

        void displayDetailedInfo() const
        {
            std::cout << "Name: " << name << '\n';
            std::cout << "Date: " << std::setw(2) << std::setfill('0') << currentDate.getDay() << '.' << std::setw(2) << std::setfill('0') << currentDate.getMonth() << '.' << currentDate.getYear() << '\n';
            std::cout << "Description: " << description << '\n';
            std::cout << "\n";
        }

        bool operator == (const Event& e2) 
        {
            return (this -> getDate().equalDate(e2.getDate()));
        }
};

class Calendar
{
    private:
        Date currentDate;
        Date calendarDate; // date on user's pc
        std::vector<Event> events;
        std::vector<std::pair<int, int>> daysInMonth;
        std::vector<std::string> monthNames = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

        int getDayOfWeek(int q, int m, int Y) 
        {
            if (m < 3) 
            {
                m += 12;
                Y--;
            }
            int j = Y / 100;
            int k = Y % 100;
            int h = (q + 13*(m+1)/5 + k + k/4 + j/4 + 5*j) % 7;
            return (h + 5) % 7 + 1; // 1 = Monday, 7 = Sunday
        }

        void setCalendarDate()
        {
            auto now = std::chrono::system_clock::now();
            std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
            std::tm* now_tm = std::localtime(&now_time_t);
            
            calendarDate.setDay(now_tm->tm_mday);
            calendarDate.setMonth(now_tm->tm_mon + 1);     // tm_mon starts with 0
            calendarDate.setYear(now_tm->tm_year + 1900);  // tm_mon is years since 1900
        }

        Event &findEventByDate(Date findByDate)
        {
            for (auto &event : events)
            {
                if (event.getDate().equalDate(findByDate))
                {
                    return event;
                }
            }
            // If no matching event is found, throw an exception
            throw std::runtime_error("Event not found\n");
        }

    public:
        // selectedDate
        void setCurrentDate(int y, int m, int d) 
        {
            currentDate = Date(y, m, d); 
        } 

        int getYear() const { return currentDate.getYear(); }
        int getMonth() const { return currentDate.getMonth(); }
        int getDay() const { return currentDate.getDay(); }

        Calendar()
        {
            setCalendarDate();
            setCurrentDate(calendarDate.getYear(), calendarDate.getMonth(), calendarDate.getDay());

            int y = calendarDate.getYear();

            daysInMonth.resize(13);
            daysInMonth[1] = {31, getDayOfWeek(1, 1, y)};
            daysInMonth[3] = {31, getDayOfWeek(1, 3, y)};
            daysInMonth[4] = {30, getDayOfWeek(1, 4, y)};
            daysInMonth[5] = {31, getDayOfWeek(1, 5, y)};
            daysInMonth[6] = {30, getDayOfWeek(1, 6, y)};
            daysInMonth[7] = {31, getDayOfWeek(1, 7, y)};
            daysInMonth[8] = {31, getDayOfWeek(1, 8, y)};
            daysInMonth[9] = {30, getDayOfWeek(1, 9, y)};
            daysInMonth[10] = {31, getDayOfWeek(1, 10, y)};
            daysInMonth[11] = {30, getDayOfWeek(1, 11, y)};
            daysInMonth[12] = {31, getDayOfWeek(1, 12, y)};

            // Leap year check for February
            if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
                daysInMonth[2] = {29, getDayOfWeek(1, 2, y)};
            else
                daysInMonth[2] = {28, getDayOfWeek(1, 2, y)};
        }

        void printCalendar(int m, int colorDay = 0)
        {
            int firstDay = daysInMonth[m].second;
            int days = daysInMonth[m].first;
            
            std::cout << '\n' << monthNames[m] << ' ' << getYear() << '\n';
            std::cout << "Mo Tu We Th Fr Sa Su \n";

            int dayNow = 1;
            for (int i = 0; i < days + firstDay - 1; i++)
            {
                if (i < firstDay - 1)
                {
                    std::cout << "   ";
                }
                else
                {
                    if (i % 7 == 0 && i != 0)
                        std::cout << "\n";
                    else if (i != firstDay - 1)
                        std::cout << " ";
            
                    if (dayNow == colorDay)
                    {
                        textColor(RED);
                        if (dayNow < 10)
                            std::cout << " " << dayNow;
                        else
                            std::cout << dayNow;
                        textColor(LIGHTGRAY);
                        dayNow++;
                    }
                    else 
                    {
                        if (dayNow < 10)
                            std::cout << " " << dayNow;
                        else
                            std::cout << dayNow;
                        dayNow++;
                    }
                }
            }
            std::cout << '\n';
        }

        // Gets valid date
        Event addEvent(Date date, const std::string& name)
        {
            if (events.size() > 100)
            {
                throw std::invalid_argument("Cant add more events");
            }
            for (const Event& event : events)
            {
                if (date.equalDate(event.getDate()))
                {
                    throw std::invalid_argument("There's already an event planned for this day\n");
                }
            }
            Event newEvent(name, "", date);
            events.push_back(newEvent);
            return newEvent;
        }


        void removeEvent(Event &deleteEvent)
        {
            auto findEvent = std::find(events.begin(), events.end(), deleteEvent);
            if (findEvent == events.end())
            {
                throw std::invalid_argument("Event not found\n");
            }
            events.erase(findEvent);
        }

        // Called when printing calendar for one month
        void displayAllMonthEventsBasic()
        {
            int i = 0;
            for (const auto &event : events)
            {
                if (event.getDate().compareMonthYear(currentDate))
                {
                    i++;
                    event.displayBasicInfo();
                }
            }
        }

        // Called in display details and redact
        void DisplayEventDetails(Event &displayEvent)
        {
            for (const auto& event : events)
            {
                if (event.getDate().equalDate(displayEvent.getDate()))
                {
                    event.displayDetailedInfo();
                    return;
                }
            }
        }

        void DisplayEventEditor(Event &editEvent)
        {
            editEvent.displayDetailedInfo();
            int mode;
            std::cout << "\n1 - Change event name\n2 - Change event date\n3 - Change event description\n4 - Delete event\n5 - Cancel\nEnter a number (1 - 5): ";
            std::cin >> mode;
            if (mode < 1 || mode > 5)
            {
                std::cout << "Error: Wrong input\n";
                return;
            }

            switch (mode) 
            {
                case 1:
                {
                    std::string newName;
                    std::cout << "Enter new name for event (up to " << MAX_EVENT_NAME << " symbols): ";
                    std::cin.ignore(); // clear the newline character
                    std::getline(std::cin, newName);

                    try 
                    {
                        editEvent.changeName(newName);
                    }
                    catch (const std::exception& e) 
                    {
                        std::cerr << "Error: " << e.what() << '\n';
                        break;
                    }
                    break;
                }
                case 2:
                {
                    Date newDate(0, 0, 0);
                    std::cout << "Enter new date for the event (DD MM YYYY): ";
                    try 
                    {
                        newDate.inputDateFromConsole();
                    }
                    catch (const std::exception& e) 
                    {
                        std::cerr << "Error: " << e.what() << '\n';
                        break;
                    }
                    editEvent.changeDate(newDate);
                    break;
                }
                case 3:
                {
                    std::string newDescription;
                    std::cout << "Enter new description (up to " << MAX_EVENT_DESC << " symbols): ";
                    std::cin.ignore(); // clear the newline character
                    std::getline(std::cin, newDescription);
                    
                    try 
                    {
                        editEvent.changeDescription(newDescription);
                    } 
                    catch (const std::exception &e) 
                    {
                        std::cerr << "Error: " << e.what();
                        break;
                    }
                    break;
                }
                case 4:
                {
                    try {
                        removeEvent(editEvent);
                    } 
                    catch (const std::exception &e) 
                    {
                        std::cerr << "Error: " << e.what();
                    }
                    break;
                }
                case 5:
                {
                    break;
                }
            }
            std::cout << "Done\n";
        }

        void nextMonth()
        {
            int d, m, y;
            d = currentDate.getDay();
            m = currentDate.getMonth();
            y = currentDate.getYear();

            m += 1;
            if (m > 12)
            {
                m = 1;
                y += 1;
            }

            setCurrentDate(y, m, d);
        }

        void prevMonth()
        {
            int d, m, y;
            d = currentDate.getDay();
            m = currentDate.getMonth();
            y = currentDate.getYear();

            m -= 1;
            if (m < 1)
            {
                m = 12;
                y -= 1;
            }

            setCurrentDate(y, m, d);
        }

        void displayMainScreen(int dayNow = 0)
        {
            if (currentDate.compareMonthYear(calendarDate))
            {
                dayNow = calendarDate.getDay();
            }

            printCalendar(getMonth(), dayNow);
            std::cout << "\nEvents scheduled for this month:\n"; 
            displayAllMonthEventsBasic();

            std::cout << "\n1 - Show calendar for previous month\n"
                      << "2 - Show calendar for next month\n"
                      << "3 - Change month and year\n"
                      << "4 - Display details or edit an event from this month\n"
                      << "5 - Schedule a new event for this month\n"
                      << "6 - Show all scheduled events\n"
                      << "7 - Exit\n";    
        }

        void run()
        {
            // Working with file
            std::ofstream createFile("events.txt", std::ios::app); // create a file
            createFile.close();

            std::ifstream readFile("events.txt", std::ios::in);
            if (!readFile.is_open()) 
            {
                printTextInColor("Couldn't open events.txt\n", RED);
                system("pause");
                return;
            }
            else 
            {
                parseFromFile(readFile);
            }
            // End of working with file

            int inputCommand = -1;
            displayMainScreen();

            while (inputCommand != 7) 
            {
                inputCommand = -1;
                do 
                {
                    std::cout << "Enter a number (1 - 7): ";
                    std::cin >> inputCommand;
                } while (inputCommand > 7 || inputCommand < 1);

                // process the input 
                switch (inputCommand)
                {
                    case 1:
                    {
                        prevMonth();
                        break;
                    }
                    case 2:
                    {
                        nextMonth();
                        break;
                    }
                    case 3:
                    {
                        std::cout << "Enter new date (DD MM YYYY): ";

                        Date inputDate;
                        try 
                        {
                            inputDate.inputDateFromConsole();
                            currentDate = inputDate;
                        } 
                        catch (const std::invalid_argument &e) 
                        {
                            std::cerr << e.what();
                            break;
                        }
                        break;
                    }
                    case 4:
                    {
                        Date inputDate = currentDate;
                        try 
                        {
                            int newEventDay;
                            std::cout << "Enter the day of the event (DD): ";
                            std::cin >> newEventDay;
                            inputDate.setDay(newEventDay);
                        } 
                        catch (const std::invalid_argument &e) 
                        {
                            std::cerr << e.what();
                            break;
                        }
                        try 
                        {
                            Event &changeEvent = findEventByDate(inputDate);
                            DisplayEventEditor(changeEvent);
                            
                            // Working with file
                            std::ofstream clearFile("events.txt", std::ofstream::trunc);
                            clearFile.close();

                            std::ofstream saveFile("events.txt", std::ios::app);
                            for (const auto& event : events)
                            {
                                saveEventToFile(event, saveFile);
                            }
                            saveFile.close();
                            // End of working with file
                            
                            std::cout << "\n\n";
                        } 
                        catch (const std::runtime_error &e) 
                        {
                            std::cerr << e.what();
                            break;
                        }
                        break; 
                    }
                    case 5:
                    {
                        Date newEventDate = currentDate;
                        Event newEvent;
                        std::string newEventName;

                        try 
                        {
                            int newEventDay;
                            std::cout << "Enter the day of the event (DD): ";
                            std::cin >> newEventDay;
                            newEventDate.setDay(newEventDay);
                        } 
                        catch (const std::invalid_argument &e) 
                        {
                            std::cerr << "Error: " << e.what();
                            break;
                        }

                        try 
                        {
                            std::cout << "Enter name of the event (up to " << MAX_EVENT_NAME << " characters): ";
                            std::cin.ignore(); // clear the newline character
                            std::getline(std::cin, newEventName);
                            newEvent.changeName(newEventName);
                        } 
                        catch (const std::exception &e) 
                        {
                            std::cerr << e.what();
                            break;
                        }

                        try 
                        {
                            newEvent = addEvent(newEventDate, newEventName);
                        } 
                        catch (const std::exception &e) 
                        {
                            std::cerr << e.what();
                            break;
                        }

                        // Working with file
                        std::ofstream saveFile("events.txt", std::ios::app);
                        if (!saveFile.is_open())
                        {
                            printTextInColor("Couldn't open events.txt\n", RED);
                            system("pause");
                            return;
                        }
                        saveEventToFile(newEvent, saveFile);
                        saveFile.close();
                        // End of working with file

                        std::cout << "Scheduled an event for ";
                        newEvent.getDate().printDate();
                        std:: cout << "\n"; 
                        break;
                    }
                    case 6:
                    {
                        std::sort(events.begin(), events.end(), 
                                [] (const Event& lhs, const Event& rhs) {
                                    return lhs.getDate() < rhs.getDate();
                                });
                        
                        for (const auto& event : events)
                        {
                            event.displayDetailedInfo();
                        }

                        break;
                    }
                    case 7:
                    {
                        return;
                        break;
                    }
                }
                if (inputCommand == 4 || inputCommand == 5 || inputCommand == 6)
                    system("pause");
                std::cout << "\n\n";
                displayMainScreen();
            }
        }

        void saveEventToFile(Event saveEvent, std::ofstream& saveFile)
        {
            Date eDate = saveEvent.getDate();
            saveFile << eDate.getDay() << ',' << eDate.getMonth() << ',' << eDate.getYear() << ','
                     << saveEvent.getName() << ',' << saveEvent.getDescription() << '\n';
        }

        void parseFromFile(std::ifstream& readFile)
        {
            Event newReadEvent;
            Date newReadDate;
            std::string token;

            while (std::getline(readFile, token, ',')) 
            {
                try 
                {
                    newReadDate.setDay(std::stoi(token));
                
                    std::getline(readFile, token, ',');
                    newReadDate.setMonth(std::stoi(token));

                    std::getline(readFile, token, ',');
                    newReadDate.setYear(std::stoi(token));
                } 
                catch (const std::exception& e) 
                {
                    std::cerr << "Error while reading from file. Couldn't convert string to int\n";
                }

                try 
                {
                    std::getline(readFile, token, ',');
                    newReadEvent.changeName(token);

                    std::getline(readFile, token);
                    newReadEvent.changeDescription(token);

                    newReadEvent.changeDate(newReadDate);
                } 
                catch (const std::invalid_argument& e) 
                {
                    std::cerr << "Error while reading from file\n";
                }

                events.push_back(newReadEvent);
            }
        }
};