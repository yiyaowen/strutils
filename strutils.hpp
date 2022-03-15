// Copyleft 2022 yiyaowen.
// https://github.com/yiyaowen/strutils

#ifndef YIYAOWEN_STRUTILS_HPP
#define YIYAOWEN_STRUTILS_HPP

#include <cstdlib>
#include <string>
#include <vector>

namespace yiyaowen {

struct strutils
{
#define FUNC_ALIAS constexpr static auto&
//-------------------------------------------------------------------------------------------------
    static std::string upper(const std::string& str)
    {
        std::string uppercase = str;
        for (auto& c : uppercase) c = std::toupper(c);
        return uppercase;
    }
//-------------------------------------------------------------------------------------------------
    static std::string lower(const std::string& str)
    {
        std::string lowercase = str;
        for (auto& c : lowercase) c = std::tolower(c);
        return lowercase;
    }
//-------------------------------------------------------------------------------------------------
private:
    template <typename T, typename T::value_type... ws>
    static T _trim(const T& str)
    {
        // char type
        using ch_t = typename T::value_type;
        // whitesace char array type
        using ws_t = std::array<ch_t, sizeof...(ws)>;

        // whitespace char candidate set
        constexpr static std::array<ws_t, 1> whitespaces = { { ws... } };

        constexpr static auto all_not_equal = [&](ch_t c1, const ws_t& c_array) -> bool
        {
            return std::all_of(c_array.begin(), c_array.end(),
                [&](ch_t c2) -> bool { return c1 != c2; });
        };

        // Find the first non-whitespace char (NOT in whitespaces).
        auto trim_start = std::find_first_of(str.begin(), str.end(),
            whitespaces.begin(), whitespaces.end(), all_not_equal);
        auto trim_end = std::find_first_of(str.rbegin(), str.rend(),
            whitespaces.begin(), whitespaces.end(), all_not_equal);

        // When there's any non-whitespace char found in string.
        if (trim_start != str.end() && trim_end != str.rend()) {
            size_t start_pos = std::distance(str.begin(), trim_start);
            size_t end_pos = std::distance(trim_end, str.rend()) - 1;
            return str.substr(start_pos, end_pos - start_pos + 1);
        }
        return {}; // Return empty string if non-whitespace char NOT found.
    }
public:
    FUNC_ALIAS trim = _trim<std::string, ' ', '\t'>;
    FUNC_ALIAS wtrim = _trim<std::wstring, L' ', L'\t'>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T>
    static std::pair<T, T> _split_once(const T& str, typename T::value_type symbol)
    {
        std::pair<T, T> kv;
        size_t symbol_pos = str.find_first_of(symbol);
        if (symbol_pos == T::npos) return kv;
        kv.first = str.substr(0, symbol_pos);
        kv.second = str.substr(symbol_pos + 1);
        return kv;
    }
public:
    FUNC_ALIAS split_once = _split_once<std::string>;
    FUNC_ALIAS wsplit_once = _split_once<std::wstring>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T>
    static std::vector<T> _split(const T& str, typename T::value_type separator)
    {
        std::vector<T> substr_array;
        std::pair<T, T> tmp_substr = { {}, str};
        while (tmp_substr.second.find(separator) != T::npos) {
            tmp_substr = _split_once<T>(tmp_substr.second, separator);
            substr_array.push_back(tmp_substr.first);
        }
        substr_array.push_back(tmp_substr.second);
        return substr_array;
    }
public:
    FUNC_ALIAS split = _split<std::string>;
    FUNC_ALIAS wsplit = _split<std::wstring>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T>
    static T _join(const std::vector<T>& str_array, const T& joint_mark)
    {
        T str;
        if (str_array.empty()) return str;
        for (size_t i = 0; i < str_array.size() - 1; ++i) {
            str.append(str_array.at(i) + joint_mark);
        }
        if (str_array.size() > 0) {
            str.append(str_array.at(str_array.size() - 1));
        }
        return str;
    }
public:
    FUNC_ALIAS join = _join<std::string>;
    FUNC_ALIAS wjoin = _join<std::wstring>;
//-------------------------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable : 4996)

    static std::wstring str_to_wstr(const std::string& str)
    {
        size_t wstr_size = mbstowcs(nullptr, str.c_str(), 0);
        std::wstring wstr(wstr_size, L' ');
        mbstowcs(wstr.data(), str.c_str(), wstr_size);
        return wstr;
    }

    static std::string wstr_to_str(const std::wstring& wstr)
    {
        size_t str_size = wcstombs(nullptr, wstr.c_str(), 0);
        std::string str(str_size, ' ');
        wcstombs(str.data(), wstr.c_str(), str_size);
        return str;
    }

#pragma warning(pop)
//-------------------------------------------------------------------------------------------------
private:
    template <typename T, typename T::value_type... lims>
    static T _dir_from_path(const T& path)
    {
        using ch_t = typename T::value_type;
        constexpr static std::array<ch_t, sizeof...(lims) + 1> limitors = { lims..., 0 };
        size_t lim_pos = path.find_last_of(limitors.data());
        // When path is file.
        if (lim_pos == T::npos) return {};
        return path.substr(0, lim_pos);
    }
public:
    FUNC_ALIAS dir_from_path = _dir_from_path<std::string, '/', '\\'>;
    FUNC_ALIAS wdir_from_path = _dir_from_path<std::wstring, L'/', L'\\'>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T, typename T::value_type... lims>
    static T _file_from_path(const T& path)
    {
        using ch_t = typename T::value_type;
        constexpr static std::array<ch_t, sizeof...(lims) + 1> limitors = { lims..., 0 };
        return path.substr(path.find_last_of(limitors.data()) + 1);
    }
public:
    FUNC_ALIAS file_from_path = _file_from_path<std::string, '/', '\\'>;
    FUNC_ALIAS wfile_from_path = _file_from_path<std::wstring, L'/', L'\\'>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T, typename T::value_type... ds>
    static T _body_of_file(const T& file)
    {
        using ch_t = typename T::value_type;
        constexpr static std::array<ch_t, sizeof...(ds) + 1> dots = { ds..., 0 };
        return file.substr(0, file.find_last_of(dots.data()));
    }
public:
    FUNC_ALIAS body_of_file = _body_of_file<std::string, '.'>;
    FUNC_ALIAS wbody_of_file = _body_of_file<std::wstring, L'.'>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T, typename T::value_type... ds>
    static T _suffix_of_file(const T& file)
    {
        using ch_t = typename T::value_type;
        constexpr static std::array<ch_t, sizeof...(ds) + 1> dots = { ds..., 0 };
        size_t dot_pos = file.find_last_of(dots.data());
        // When file is body.
        if (dot_pos == T::npos) return {};
        return file.substr(dot_pos + 1);
    }
public:
    FUNC_ALIAS suffix_of_file = _suffix_of_file<std::string, '.'>;
    FUNC_ALIAS wsuffix_of_file = _suffix_of_file<std::wstring, L'.'>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T>
    static T _merge_path(const T& dir, const T& file, typename T::value_type lim)
    {
        return (dir.at(dir.size() - 1) == lim) ? (dir + file) : (dir + T(1, lim) + file);
    }
public:
    FUNC_ALIAS merge_path = _merge_path<std::string>;
    FUNC_ALIAS wmerge_path = _merge_path<std::wstring>;
//-------------------------------------------------------------------------------------------------
private:
    template <typename T>
    static typename T::value_type* _ptr_from_str(const T& str)
    {
        using ch_t = typename T::value_type;
        ch_t* str_ptr = new ch_t[str.size() + 1];
        memcpy(str_ptr, str.data(), str.size() * sizeof(ch_t));
        str_ptr[str.size()] = 0; // end with null
        return str_ptr;
    }
public:
    FUNC_ALIAS ptr_from_str = _ptr_from_str<std::string>;
    FUNC_ALIAS wptr_from_wstr = _ptr_from_str<std::wstring>;
//-------------------------------------------------------------------------------------------------
#undef FUNC_ALIAS
};

} // yiyaowen

#endif // YIYAOWEN_STRUTILS_HPP
