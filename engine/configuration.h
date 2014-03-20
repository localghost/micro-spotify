#ifndef ENGINE_CONFIGURATION_H
#define ENGINE_CONFIGURATION_H

struct configuration
{
public:
    static configuration from_file(const boost::filesystem::path& path);

    boost::filesystem cache;
    std::string username;
    encrypted_string password;
};

#endif
