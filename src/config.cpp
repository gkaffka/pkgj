#include "config.hpp"

#include <fmt/format.h>

#include "file.hpp"
#include "pkgi.hpp"

static constexpr char default_psv_games_url[] = {
        0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6e, 0x6f, 0x70, 0x61,
        0x79, 0x73, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x63, 0x6f,
        0x6d, 0x2f, 0x74, 0x73, 0x76, 0x2f, 0x50, 0x53, 0x56, 0x5f, 0x47,
        0x41, 0x4d, 0x45, 0x53, 0x2e, 0x74, 0x73, 0x76, 0x00};
static constexpr char default_psv_dlcs_url[] = {
        0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6e, 0x6f, 0x70, 0x61,
        0x79, 0x73, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x63, 0x6f,
        0x6d, 0x2f, 0x74, 0x73, 0x76, 0x2f, 0x50, 0x53, 0x56, 0x5f, 0x44,
        0x4c, 0x43, 0x53, 0x2e, 0x74, 0x73, 0x76, 0x00};
static constexpr char default_psx_games_url[] = {
        0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6e, 0x6f, 0x70, 0x61,
        0x79, 0x73, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x63, 0x6f,
        0x6d, 0x2f, 0x74, 0x73, 0x76, 0x2f, 0x50, 0x53, 0x58, 0x5f, 0x47,
        0x41, 0x4d, 0x45, 0x53, 0x2e, 0x74, 0x73, 0x76, 0x00};
static constexpr char default_psp_games_url[] = {
        0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6e, 0x6f, 0x70, 0x61,
        0x79, 0x73, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x63, 0x6f,
        0x6d, 0x2f, 0x74, 0x73, 0x76, 0x2f, 0x50, 0x53, 0x50, 0x5f, 0x47,
        0x41, 0x4d, 0x45, 0x53, 0x2e, 0x74, 0x73, 0x76, 0x00};
static constexpr char default_psm_games_url[] = {
        0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6e, 0x6f, 0x70, 0x61,
        0x79, 0x73, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x63, 0x6f,
        0x6d, 0x2f, 0x74, 0x73, 0x76, 0x2f, 0x50, 0x53, 0x4d, 0x5f, 0x47,
        0x41, 0x4d, 0x45, 0x53, 0x2e, 0x74, 0x73, 0x76, 0x00};
static constexpr char default_comppack_url[] = {
        0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x70, 0x72, 0x6f, 0x78,
        0x79, 0x2e, 0x6e, 0x6f, 0x70, 0x61, 0x79, 0x73, 0x74, 0x61, 0x74,
        0x69, 0x6f, 0x6e, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x68, 0x74, 0x74,
        0x70, 0x73, 0x3a, 0x2f, 0x2f, 0x67, 0x69, 0x74, 0x6c, 0x61, 0x62,
        0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x6e, 0x6f, 0x70, 0x61, 0x79, 0x73,
        0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x5f, 0x72, 0x65, 0x70, 0x6f,
        0x73, 0x2f, 0x6e, 0x70, 0x73, 0x5f, 0x63, 0x6f, 0x6d, 0x70, 0x61,
        0x74, 0x69, 0x5f, 0x70, 0x61, 0x63, 0x6b, 0x73, 0x2f, 0x72, 0x61,
        0x77, 0x2f, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2f, 0x00};

static char* skipnonws(char* text, char* end)
{
    while (text < end && *text != ' ' && *text != '\n' && *text != '\r')
    {
        text++;
    }
    return text;
}

static char* skipws(char* text, char* end)
{
    while (text < end && (*text == ' ' || *text == '\n' || *text == '\r'))
    {
        text++;
    }
    return text;
}

static DbSort parse_sort(const char* value, DbSort sort)
{
    if (pkgi_stricmp(value, "title") == 0)
    {
        return SortByTitle;
    }
    else if (pkgi_stricmp(value, "region") == 0)
    {
        return SortByRegion;
    }
    else if (pkgi_stricmp(value, "name") == 0)
    {
        return SortByName;
    }
    else if (pkgi_stricmp(value, "size") == 0)
    {
        return SortBySize;
    }
    else if (pkgi_stricmp(value, "date") == 0)
    {
        return SortByDate;
    }
    else
    {
        return sort;
    }
}

static DbSortOrder parse_order(const char* value, DbSortOrder order)
{
    if (pkgi_stricmp(value, "asc") == 0)
    {
        return SortAscending;
    }
    else if (pkgi_stricmp(value, "desc") == 0)
    {
        return SortDescending;
    }
    else
    {
        return order;
    }
}

static DbFilter parse_filter(char* value, uint32_t filter)
{
    uint32_t result = 0;

    char* start = value;
    for (;;)
    {
        char ch = *value;
        if (ch == 0 || ch == ',')
        {
            *value = 0;
            if (pkgi_stricmp(start, "ASA") == 0)
            {
                result |= DbFilterRegionASA;
            }
            else if (pkgi_stricmp(start, "EUR") == 0)
            {
                result |= DbFilterRegionEUR;
            }
            else if (pkgi_stricmp(start, "JPN") == 0)
            {
                result |= DbFilterRegionJPN;
            }
            else if (pkgi_stricmp(start, "USA") == 0)
            {
                result |= DbFilterRegionUSA;
            }
            else
            {
                return static_cast<DbFilter>(filter);
            }
            if (ch == 0)
            {
                break;
            }
            value++;
            start = value;
        }
        else
        {
            value++;
        }
    }

    return static_cast<DbFilter>(result);
}

Config pkgi_load_config()
{
    try
    {
        Config config{};

        config.games_url = default_psv_games_url;
        config.dlcs_url = default_psv_dlcs_url;
        config.psm_games_url = default_psm_games_url;
        config.psx_games_url = default_psx_games_url;
        config.psp_games_url = default_psp_games_url;
        config.comppack_url = default_comppack_url;
        config.sort = SortByName;
        config.order = SortAscending;
        config.filter = DbFilterAll;
        config.install_psp_psx_location = "ux0:";

        auto const path =
                fmt::format("{}/config.txt", pkgi_get_config_folder());
        LOGF("config location: {}", path);

        if (!pkgi_file_exists(path))
            return config;

        auto data = pkgi_load(path);
        data.push_back('\n');

        LOG("config.txt loaded, parsing");
        auto text = reinterpret_cast<char*>(data.data());
        const auto end = text + data.size();

        while (text < end)
        {
            const auto key = text;

            text = skipnonws(text, end);
            if (text == end)
                break;

            *text++ = 0;

            text = skipws(text, end);
            if (text == end)
                break;

            const auto value = text;

            text = skipnonws(text, end);
            if (text == end)
                break;

            *text++ = 0;

            text = skipws(text, end);

            if (pkgi_stricmp(key, "url") == 0 ||
                pkgi_stricmp(key, "url_games") == 0)
                config.games_url = value;
            else if (pkgi_stricmp(key, "url_dlcs") == 0)
                config.dlcs_url = value;
            else if (pkgi_stricmp(key, "url_psm_games") == 0)
                config.psm_games_url = value;
            else if (pkgi_stricmp(key, "url_psx_games") == 0)
                config.psx_games_url = value;
            else if (pkgi_stricmp(key, "url_psp_games") == 0)
                config.psp_games_url = value;
            else if (pkgi_stricmp(key, "url_comppack") == 0)
                config.comppack_url = value;
            else if (pkgi_stricmp(key, "sort") == 0)
                config.sort = parse_sort(value, SortByName);
            else if (pkgi_stricmp(key, "order") == 0)
                config.order = parse_order(value, SortAscending);
            else if (pkgi_stricmp(key, "filter") == 0)
                config.filter = parse_filter(value, DbFilterAll);
            else if (pkgi_stricmp(key, "no_version_check") == 0)
                config.no_version_check = 1;
            else if (pkgi_stricmp(key, "install_psp_as_pbp") == 0)
                config.install_psp_as_pbp = 1;
            else if (pkgi_stricmp(key, "install_psp_psx_location") == 0)
                config.install_psp_psx_location = value;
            else if (
                    pkgi_stricmp(key, "psm_disclaimer_yes_i_read_the_readme") ==
                    0)
                config.psm_readme_disclaimer =
                        (pkgi_stricmp(value, "NoPsmDrm") == 0);
        }
        return config;
    }
    catch (const std::exception& e)
    {
        throw formatEx<std::runtime_error>(
                "Failed to load config:\n{}", e.what());
    }
}

static const char* sort_str(DbSort sort)
{
    switch (sort)
    {
    case SortByTitle:
        return "title";
    case SortByRegion:
        return "region";
    case SortByName:
        return "name";
    case SortBySize:
        return "size";
    case SortByDate:
        return "date";
    }
    return "";
}

static const char* order_str(DbSortOrder order)
{
    switch (order)
    {
    case SortAscending:
        return "asc";
    case SortDescending:
        return "desc";
    }
    return "";
}

void pkgi_save_config(const Config& config)
{
    char data[4096];
    int len = 0;
    if (!config.games_url.empty() && config.games_url != default_psv_games_url)
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "url_games %s\n",
                config.games_url.c_str());
    if (!config.dlcs_url.empty() && config.dlcs_url != default_psv_dlcs_url)
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "url_dlcs %s\n",
                config.dlcs_url.c_str());
    if (!config.psm_games_url.empty() &&
        config.psm_games_url != default_psm_games_url)
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "url_psm_games %s\n",
                config.psm_games_url.c_str());
    if (!config.psx_games_url.empty() &&
        config.psx_games_url != default_psx_games_url)
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "url_psx_games %s\n",
                config.psx_games_url.c_str());
    if (!config.psp_games_url.empty() &&
        config.psp_games_url != default_psp_games_url)
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "url_psp_games %s\n",
                config.psp_games_url.c_str());
    if (!config.comppack_url.empty() &&
        config.comppack_url != default_comppack_url)
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "url_comppack %s\n",
                config.comppack_url.c_str());
    if (!config.install_psp_psx_location.empty())
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "install_psp_psx_location %s\n",
                config.install_psp_psx_location.c_str());
    if (config.psm_readme_disclaimer)
        len += pkgi_snprintf(
                data + len,
                sizeof(data) - len,
                "psm_disclaimer_yes_i_read_the_readme NoPsmDrm\n");
    len += pkgi_snprintf(
            data + len, sizeof(data) - len, "sort %s\n", sort_str(config.sort));
    len += pkgi_snprintf(
            data + len,
            sizeof(data) - len,
            "order %s\n",
            order_str(config.order));
    len += pkgi_snprintf(data + len, sizeof(data) - len, "filter ");
    const char* sep = "";
    if (config.filter & DbFilterRegionASA)
    {
        len += pkgi_snprintf(data + len, sizeof(data) - len, "%sASA", sep);
        sep = ",";
    }
    if (config.filter & DbFilterRegionEUR)
    {
        len += pkgi_snprintf(data + len, sizeof(data) - len, "%sEUR", sep);
        sep = ",";
    }
    if (config.filter & DbFilterRegionJPN)
    {
        len += pkgi_snprintf(data + len, sizeof(data) - len, "%sJPN", sep);
        sep = ",";
    }
    if (config.filter & DbFilterRegionUSA)
    {
        len += pkgi_snprintf(data + len, sizeof(data) - len, "%sUSA", sep);
        sep = ",";
    }
    len += pkgi_snprintf(data + len, sizeof(data) - len, "\n");

    if (config.no_version_check)
    {
        len += pkgi_snprintf(
                data + len, sizeof(data) - len, "no_version_check 1\n");
    }

    if (config.install_psp_as_pbp)
    {
        len += pkgi_snprintf(
                data + len, sizeof(data) - len, "install_psp_as_pbp 1\n");
    }

    pkgi_save(
            fmt::format("{}/config.txt", pkgi_get_config_folder()), data, len);
}
