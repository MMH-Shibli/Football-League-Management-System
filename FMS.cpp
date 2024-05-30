#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

class Player {
public:
    int id;
    string name;
    string position;
    int jersey;

    Player(int id, string name, string position, int jersey) 
        : id(id), name(name), position(position), jersey(jersey) {}

    string toString() const {
        return to_string(id) + "," + name + "," + position + "," + to_string(jersey);
    }

    static Player fromString(const string &str) {
        size_t pos1 = str.find(',');
        size_t pos2 = str.find(',', pos1 + 1);
        size_t pos3 = str.find(',', pos2 + 1);

        int id = stoi(str.substr(0, pos1));
        string name = str.substr(pos1 + 1, pos2 - pos1 - 1);
        string position = str.substr(pos2 + 1, pos3 - pos2 - 1);
        int jersey = stoi(str.substr(pos3 + 1));

        return Player(id, name, position, jersey);
    }
};

class Team {
public:
    string name;
    vector<Player> players;

    Team(string name) : name(name) {}

    void addPlayer(const Player &player) {
        if (players.size() < 5) {
            players.push_back(player);
        } else {
            cout << "Cannot add more players. Team already has 5 players." << endl;
        }
    }

    void listPlayers() const {
        if (players.empty()) {
            cout << "No players in the team." << endl;
        } else {
            cout << "Players in " << name << ":" << endl;
            for (const auto &player : players) {
                cout << "ID: " << player.id << ", Name: " << player.name 
                     << ", Position: " << player.position << ", Jersey: " << player.jersey << endl;
            }
        }
    }

    string toString() const {
        string result = name + "\n";
        for (const auto &player : players) {
            result += player.toString() + "\n";
        }
        return result;
    }

    static Team fromString(const string &str) {
        size_t pos = str.find('\n');
        string name = str.substr(0, pos);
        Team team(name);
        string playersStr = str.substr(pos + 1);
        size_t start = 0, end;
        while ((end = playersStr.find('\n', start)) != string::npos) {
            team.addPlayer(Player::fromString(playersStr.substr(start, end - start)));
            start = end + 1;
        }
        return team;
    }
};

class Department {
public:
    string name;
    vector<Team> teams;

    Department(string name) : name(name) {}

    void addTeam(const Team &team) {
        teams.push_back(team);
    }

    void listTeams() const {
        if (teams.empty()) {
            cout << "No teams in the department." << endl;
        } else {
            cout << "Teams in " << name << " department:" << endl;
            for (const auto &team : teams) {
                cout << "Team: " << team.name << endl;
            }
        }
    }

    Team* findTeam(const string &name) {
        auto it = find_if(teams.begin(), teams.end(), [&name](const Team &team) { return team.name == name; });
        if (it != teams.end()) {
            return &(*it);
        } else {
            return nullptr;
        }
    }

    string toString() const {
        string result = name + "\n";
        for (const auto &team : teams) {
            result += team.toString() + "\n";
        }
        return result;
    }

    static Department fromString(const string &str) {
        size_t pos = str.find('\n');
        string name = str.substr(0, pos);
        Department department(name);
        string teamsStr = str.substr(pos + 1);
        size_t start = 0, end;
        while ((end = teamsStr.find('\n', start)) != string::npos) {
            department.addTeam(Team::fromString(teamsStr.substr(start, end - start)));
            start = end + 1;
        }
        return department;
    }
};

class League {
public:
    vector<Department> departments;

    void addDepartment(const Department &department) {
        departments.push_back(department);
    }

    Department* findDepartment(const string &name) {
        auto it = find_if(departments.begin(), departments.end(), [&name](const Department &department) { return department.name == name; });
        if (it != departments.end()) {
            return &(*it);
        } else {
            return nullptr;
        }
    }

    void saveToFile(const string &filename) const {
        ofstream outFile(filename);
        for (const auto &department : departments) {
            outFile << department.toString() << "\n";
        }
        outFile.close();
    }

    void loadFromFile(const string &filename) {
        ifstream inFile(filename);
        string line, departmentStr;
        while (getline(inFile, line)) {
            if (line.empty()) {
                continue;
            }
            if (departmentStr.empty()) {
                departmentStr = line;
            } else if (line.find(',') != string::npos || line.find('\n') == string::npos) {
                departmentStr += "\n" + line;
            } else {
                addDepartment(Department::fromString(departmentStr));
                departmentStr = line;
            }
        }
        if (!departmentStr.empty()) {
            addDepartment(Department::fromString(departmentStr));
        }
        inFile.close();
    }
};

class Match {
public:
    string team1;
    string team2;
    double start;
    double end;
    string venue;
    double distance;

    Match(string team1, string team2, double start, double end, string venue, double distance)
        : team1(team1), team2(team2), start(start), end(end), venue(venue), distance(distance) {}
};

void showMainMenu();
void showDepartmentMenu();
void showTeamMenu();
void showPlayerMenu();
void activitySelectionSort();
void optimizeTravelSchedule();

int main() {
    League league;
    league.loadFromFile("RecordBook.txt");

    // Initialize departments if not already present
    if (league.findDepartment("IRE") == nullptr) {
        league.addDepartment(Department("IRE"));
    }
    if (league.findDepartment("EdTech") == nullptr) {
        league.addDepartment(Department("EdTech"));
    }

    int mainChoice, departmentChoice, teamChoice, playerID = 1;
    string departmentName, teamName, playerName, position;
    int jersey;

    while (true) {
        showMainMenu();
        cin >> mainChoice;

        switch (mainChoice) {
            case 1: // Select Department
                showDepartmentMenu();
                cin >> departmentChoice;
                departmentName = (departmentChoice == 1) ? "IRE" : (departmentChoice == 2) ? "EdTech" : "";

                if (departmentName.empty()) {
                    cout << "Invalid choice. Please try again." << endl;
                    break;
                }

                if (auto* department = league.findDepartment(departmentName)) {
                    while (true) {
                        showTeamMenu();
                        cin >> teamChoice;

                        if (teamChoice == 1) { // Add Team
                            if (department->teams.size() >= 5) {
                                cout << "Maximum number of teams in " << departmentName << " department reached." << endl;
                                break;
                            }
                            cout << "Enter team name: ";
                            cin.ignore();
                            getline(cin, teamName);
                            department->addTeam(Team(teamName));
                            league.saveToFile("RecordBook.txt");
                        } else if (teamChoice == 2) { // Manage Team
                            cout << "Enter team name: ";
                            cin.ignore();
                            getline(cin, teamName);
                            if (auto* team = department->findTeam(teamName)) {
                                while (true) {
                                    showPlayerMenu();
                                    cin >> mainChoice;

                                    if (mainChoice == 1) { // Add Player
                                        if (team->players.size() >= 5) {
                                            cout << "Cannot add more players. Team already has 5 players." << endl;
                                            break;
                                        }
                                        cout << "Enter player name: ";
                                        cin.ignore();
                                        getline(cin, playerName);
                                        cout << "Enter player position: ";
                                        getline(cin, position);
                                        cout << "Enter player jersey number: ";
                                        cin >> jersey;
                                        team->addPlayer(Player(playerID++, playerName, position, jersey));
                                        league.saveToFile("RecordBook.txt");
                                    } else if (mainChoice == 2) { // List Players
                                        team->listPlayers();
                                    } else if (mainChoice == 3) { // Back to Team Menu
                                        break;
                                    } else {
                                        cout << "Invalid choice. Please try again." << endl;
                                    }
                                }
                            } else {
                                cout << "Team not found." << endl;
                            }
                        } else if (teamChoice == 3) { // List Teams
                            department->listTeams();
                        } else if (teamChoice == 4) { // Back to Main Menu
                            break;
                        } else {
                            cout << "Invalid choice. Please try again." << endl;
                        }
                    }
                } else {
                    cout << "Department not found." << endl;
                }
                break;

            case 2: // List All Departments and Teams
                for (const auto &department : league.departments) {
                    department.listTeams();
                }
                break;

            case 3: // Activity Selection Sort
                activitySelectionSort();
                break;

            case 4: // Optimize Travel Schedule
                optimizeTravelSchedule();
                break;

            case 5: // Exit
                league.saveToFile("RecordBook.txt");
                return 0;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}

void showMainMenu() {
    cout << "\nFootball League Management System" << endl;
    cout << "1. Select Department" << endl;
    cout << "2. Show point table and fixture" << endl;
    cout << "3. Activity Selection Sort" << endl;
    cout << "4. Optimize Travel Schedule" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";
}

void showDepartmentMenu() {
    cout << "\nSelect Department" << endl;
    cout << "1. IRE" << endl;
    cout << "2. EdTech" << endl;
    cout << "Enter your choice: ";
}

void showTeamMenu() {
    cout << "\nTeam Management" << endl;
    cout << "1. Add Team" << endl;
    cout << "2. Manage Team" << endl;
    cout << "3. List Teams" << endl;
    cout << "4. Back to Main Menu" << endl;
    cout << "Enter your choice: ";
}

void showPlayerMenu() {
    cout << "\nPlayer Management" << endl;
    cout << "1. Add Player" << endl;
    cout << "2. List Players" << endl;
    cout << "3. Back to Team Menu" << endl;
    cout << "Enter your choice: ";
}

void activitySelectionSort() {
    int n;
    cout << "\nActivity Selection Sort - Enter match details" << endl;
    cout << "Number of matches (minimum 4): ";
    cin >> n;

    if (n < 4) {
        cout << "Minimum number of matches is 4. Please try again." << endl;
        return;
    }

    vector<Match> matches;
    string team1, team2, venue;
    double start, end, distance;

    for (int i = 0; i < n; ++i) {
        cout << "\nMatch " << i + 1 << endl;
        cout << "Enter team 1: ";
        cin.ignore();
        getline(cin, team1);
        cout << "Enter team 2: ";
        getline(cin, team2);
        cout << "Enter start time: ";
        cin >> start;
        cout << "Enter end time: ";
        cin >> end;
        cout << "Enter venue: ";
        cin.ignore();
        getline(cin, venue);
        cout << "Enter distance from previous venue: ";
        cin >> distance;
        matches.emplace_back(team1, team2, start, end, venue, distance);
    }

    // Sort matches based on end times
    sort(matches.begin(), matches.end(), [](const Match &a, const Match &b) {
        return a.end < b.end;
    });

    cout << "\nMatches to watch:" << endl;
    double lastEndTime = -1;

    for (const auto &match : matches) {
        if (match.start >= lastEndTime) {
            cout << match.team1 << " vs " << match.team2 << " from " << match.start << " to " << match.end 
                 << " at " << match.venue << ", distance: " << match.distance << endl;
            lastEndTime = match.end;
        }
    }
}

void optimizeTravelSchedule() {
    int n;
    double maxTravelTime;

    cout << "\nOptimize Travel Schedule" << endl;
    cout << "Enter number of venues: ";
    cin >> n;

    if (n < 2) {
        cout << "Minimum number of venues is 2. Please try again." << endl;
        return;
    }

    cout << "Enter maximum travel time: ";
    cin >> maxTravelTime;

    vector<Match> matches;
    string venue;
    double distance, time;

    for (int i = 0; i < n; ++i) {
        cout << "\nVenue " << i + 1 << endl;
        cout << "Enter venue name: ";
        cin.ignore();
        getline(cin, venue);
        cout << "Enter distance from previous venue: ";
        cin >> distance;
        cout << "Enter time to reach this venue: ";
        cin >> time;
        matches.emplace_back("", "", 0, 0, venue, distance);
        matches.back().distance = time;
    }

    // 0-1 Knapsack to find the best venues within maxTravelTime
    vector<vector<double>> dp(n + 1, vector<double>(maxTravelTime + 1, 0));
    vector<vector<bool>> selected(n + 1, vector<bool>(maxTravelTime + 1, false));

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= maxTravelTime; ++j) {
            if (matches[i - 1].distance <= j) {
                if (matches[i - 1].distance + dp[i - 1][j - static_cast<int>(matches[i - 1].distance)] > dp[i - 1][j]) {
                    dp[i][j] = matches[i - 1].distance + dp[i - 1][j - static_cast<int>(matches[i - 1].distance)];
                    selected[i][j] = true;
                } else {
                    dp[i][j] = dp[i - 1][j];
                }
            } else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }

    // Backtracking to find the selected venues
    vector<string> chosenVenues;
    double totalDistance = 0;
    for (int i = n, j = maxTravelTime; i > 0 && j > 0; --i) {
        if (selected[i][j]) {
            chosenVenues.push_back(matches[i - 1].venue);
            totalDistance += matches[i - 1].distance;
            j -= static_cast<int>(matches[i - 1].distance);
        }
    }

    reverse(chosenVenues.begin(), chosenVenues.end());

    cout << "\nOptimal Travel Schedule:" << endl;
    for (const auto &venue : chosenVenues) {
        cout << "Venue: " << venue << endl;
    }
    cout << "Total distance traveled: " << totalDistance << endl;
}