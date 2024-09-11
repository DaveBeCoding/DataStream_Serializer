#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

// A simple User data structure for serialization
struct User {
    std::string name;
    uint32_t age;
    std::vector<uint8_t> access_levels;  // Example of variable-length data (e.g., permissions)

    // Constructor
    User(std::string n, uint32_t a, std::vector<uint8_t> levels)
        : name(std::move(n)), age(a), access_levels(std::move(levels)) {}
};

// Serialization function
void serialize_user(const User& user, const std::string& file_name) {
    std::ofstream ofs(file_name, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Could not open file for writing.");
    }

    // Serialize the name (length followed by characters)
    uint32_t name_len = user.name.size();
    ofs.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
    ofs.write(user.name.c_str(), name_len);

    // Serialize the age
    ofs.write(reinterpret_cast<const char*>(&user.age), sizeof(user.age));

    // Serialize the access_levels (length followed by levels)
    uint32_t levels_len = user.access_levels.size();
    ofs.write(reinterpret_cast<const char*>(&levels_len), sizeof(levels_len));
    ofs.write(reinterpret_cast<const char*>(user.access_levels.data()), levels_len);

    ofs.close();
    std::cout << "User serialized to " << file_name << std::endl;
}

// Deserialization function
std::unique_ptr<User> deserialize_user(const std::string& file_name) {
    std::ifstream ifs(file_name, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Could not open file for reading.");
    }

    // Deserialize the name
    uint32_t name_len;
    ifs.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
    std::string name(name_len, '\0');  // Pre-allocate string memory
    ifs.read(&name[0], name_len);

    // Deserialize the age
    uint32_t age;
    ifs.read(reinterpret_cast<char*>(&age), sizeof(age));

    // Deserialize the access_levels
    uint32_t levels_len;
    ifs.read(reinterpret_cast<char*>(&levels_len), sizeof(levels_len));
    std::vector<uint8_t> access_levels(levels_len);
    ifs.read(reinterpret_cast<char*>(access_levels.data()), levels_len);

    ifs.close();
    std::cout << "User deserialized from " << file_name << std::endl;
    
    return std::make_unique<User>(name, age, std::move(access_levels));
}

int main() {
    try {
        // Creating a user
        User user1("Alice", 30, {1, 2, 3, 4, 5});

        // Serialize the user
        serialize_user(user1, "user_data.bin");

        // Deserialize the user
        std::unique_ptr<User> deserialized_user = deserialize_user("user_data.bin");

        // Output the deserialized data
        std::cout << "Deserialized User Name: " << deserialized_user->name << std::endl;
        std::cout << "Deserialized User Age: " << deserialized_user->age << std::endl;
        std::cout << "Deserialized Access Levels: ";
        for (auto level : deserialized_user->access_levels) {
            std::cout << static_cast<int>(level) << " ";
        }
        std::cout << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}