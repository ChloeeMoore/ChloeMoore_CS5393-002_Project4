#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <set>
using namespace std;

class SocialNetwork {
public:
    unordered_map<string, vector<string>> adjList;

    void loadFromCSV(const string& filename) {
        ifstream file(filename);
        string line, user, friends;

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, user, ',');
            getline(ss, friends);

            stringstream fss(friends);
            string friendName;
            while (getline(fss, friendName, ';')) {
                adjList[user].push_back(friendName);
                adjList[friendName]; // Ensure all users are in the graph
            }
        }
    }

    vector<pair<string, int>> suggestFriendsByDegree(const string& user) {
        unordered_map<string, int> degreeMap;
        unordered_set<string> visited;
        unordered_set<string> directFriends(adjList[user].begin(), adjList[user].end());
        directFriends.insert(user);

        queue<pair<string, int>> q;
        q.push({user, 0});
        visited.insert(user);

        while (!q.empty()) {
            auto [current, depth] = q.front(); q.pop();
            if (depth >= 3) continue; // limit search depth to 3 hops

            for (const string& neighbor : adjList[current]) {
                if (!visited.count(neighbor)) {
                    visited.insert(neighbor);
                    if (!directFriends.count(neighbor)) {
                        degreeMap[neighbor]++;
                    }
                    q.push({neighbor, depth + 1});
                }
            }
        }

        vector<pair<string, int>> suggestions(degreeMap.begin(), degreeMap.end());
        sort(suggestions.begin(), suggestions.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
        if (suggestions.size() > 5) suggestions.resize(5);
        return suggestions;
    }

    int degreeOfSeparation(const string& user1, const string& user2) {
        unordered_map<string, int> visited;
        queue<string> q;
        q.push(user1);
        visited[user1] = 0;

        while (!q.empty()) {
            string current = q.front(); q.pop();
            for (const auto& neighbor : adjList[current]) {
                if (!visited.count(neighbor)) {
                    visited[neighbor] = visited[current] + 1;
                    if (neighbor == user2) return visited[neighbor];
                    q.push(neighbor);
                }
            }
        }
        return -1; // Not connected
    }

    void dfs(const string& user, unordered_set<string>& visited, vector<string>& component) {
        visited.insert(user);
        component.push_back(user);
        for (const auto& neighbor : adjList[user]) {
            if (!visited.count(neighbor)) dfs(neighbor, visited, component);
        }
    }

    vector<vector<string>> getConnectedComponents() {
        unordered_set<string> visited;
        vector<vector<string>> components;
        for (const auto& pair : adjList) {
            if (!visited.count(pair.first)) {
                vector<string> component;
                dfs(pair.first, visited, component);
                components.push_back(component);
            }
        }
        sort(components.begin(), components.end(), [](auto& a, auto& b) {
            return a.size() > b.size();
        });
        if (components.size() > 5) components.resize(5);
        return components;
    }

    vector<pair<string, int>> mostInfluentialUsers() {
        unordered_map<string, int> centrality;
        for (const auto& pair : adjList) {
            centrality[pair.first] = pair.second.size();
        }
        vector<pair<string, int>> ranked(centrality.begin(), centrality.end());
        sort(ranked.begin(), ranked.end(), [](auto& a, auto& b) {
            return a.second > b.second;
        });
        if (ranked.size() > 5) ranked.resize(5);
        return ranked;
    }

    void basicStats() {
        int totalUsers = adjList.size();
        int totalConnections = 0;
        vector<pair<string, int>> connectionCounts;

        for (const auto& pair : adjList) {
            int count = pair.second.size();
            totalConnections += count;
            connectionCounts.push_back({pair.first, count});
        }

        sort(connectionCounts.begin(), connectionCounts.end(), [](auto& a, auto& b) {
            return a.second > b.second;
        });

        cout << "Total Users: " << totalUsers << endl;
        cout << "Average Connections/User: " << (double)totalConnections / totalUsers << endl;
        cout << "Top 10 Users by Connections:\n";
        for (int i = 0; i < min(10, (int)connectionCounts.size()); ++i) {
            cout << connectionCounts[i].first << " (" << connectionCounts[i].second << ")\n";
        }
    }
};

int main() {
    SocialNetwork sn;
    sn.loadFromCSV("Dataset.csv");

    int choice;
    string user1, user2;
    do {
        cout << "\n--- Social Network Analysis Tool ---\n";
        cout << "1. Friend suggestions based on mutual connections\n";
        cout << "2. Degree of separation between two users\n";
        cout << "3. Identify top 5 strongly connected components\n";
        cout << "4. Analyze top 5 influential users (centrality)\n";
        cout << "5. Basic network statistics\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter username: ";
                getline(cin, user1);
                for (auto& suggestion : sn.suggestFriendsByDegree(user1)) {
                    cout << suggestion.first << " (" << suggestion.second << " distant mutuals)\n";
                }
                break;
            case 2:
                cout << "Enter first user: ";
                getline(cin, user1);
                cout << "Enter second user: ";
                getline(cin, user2);
                cout << "Degree of Separation: " << sn.degreeOfSeparation(user1, user2) << endl;
                break;
            case 3:
                int id;
                id = 1;
                for (auto& comp : sn.getConnectedComponents()) {
                    cout << "Component " << id++ << " (" << comp.size() << " users): ";
                    for (auto& user : comp) cout << user << " ";
                    cout << "\n";
                }
                break;
            case 4:
                for (auto& user : sn.mostInfluentialUsers()) {
                    cout << user.first << " (" << user.second << " connections)\n";
                }
                break;
            case 5:
                sn.basicStats();
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
