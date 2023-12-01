#include <cstdint>
#include <cstdio>
#include <fstream>

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif


#include <boost/json/src.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/blueprint/asserts.hpp>


boost::json::value read_boost_json (std::string input_file_name) {

    std::ifstream input_file(input_file_name.c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '" << input_file_name << "'" << std::endl;
        UNREACHABLE("Could not open input file!");
    }

    boost::json::stream_parser p;
    boost::json::error_code ec;
    while (!input_file.eof()) {
        char input_string[256];
        input_file.read(input_string, sizeof(input_string) - 1);
        input_string[input_file.gcount()] = '\0';
        p.write(input_string, ec);
        if (ec) {
            std::cerr << "JSON parsing of public input failed" << std::endl;
            return 1;
        }
    }
    p.finish(ec);
    if (ec) {
        std::cerr << "JSON parsing of public input failed" << std::endl;
        return 1;
    }
    boost::json::value input_json_value = p.release();
    if (!input_json_value.is_array()) {
        std::cerr << "Array of arguments is expected in JSON file" << std::endl;
        return 1;
    }

    for (std::size_t i = 0; i < input_json_value.as_array().size(); i++) {
        ASSERT_MSG(input_json_value.as_array()[i].is_object(), "input must containt json objects");
    }

    return input_json_value;
}

template<typename OperatingFieldType>
typename OperatingFieldType::value_type parse_scalar(const boost::json::value &json_value) {

    const std::size_t buflen = 256;
    char buf[buflen];
    std::size_t numlen = 0;

    typename OperatingFieldType::extended_integral_type integral_res;
    typename OperatingFieldType::value_type res;

    switch (json_value.kind()) {
    case boost::json::kind::int64:
        res = json_value.as_int64();
        return res;
    case boost::json::kind::uint64:
        res = json_value.as_int64();
        return res;
    case boost::json::kind::string: {
        numlen = json_value.as_string().size();
        if (numlen > buflen - 1) {
            std::cerr << "json_value " << json_value.as_string() << " exceeds buffer size (" << buflen - 1 << ")\n";
            UNREACHABLE("json_value size exceeds buffer size");
        }
        json_value.as_string().copy(buf, numlen);
        buf[numlen] = '\0';
        integral_res = typename OperatingFieldType::extended_integral_type(buf);
        if (integral_res >= OperatingFieldType::modulus) {
            std::cerr << "input value:   " << integral_res << "\n";
            std::cerr << "field_modulus: " << OperatingFieldType::modulus << "\n";
            UNREACHABLE("Input does not fit into OperatingFieldType");
        }
        res = integral_res;
        return res;
    }
    default:
        UNREACHABLE("only integers and strings are supported");
    }
}

template<typename OperatingFieldType>
typename OperatingFieldType::value_type read_field(boost::json::value& input_json_value, std::size_t position) {

    const boost::json::object &current_value = input_json_value.as_array()[position].as_object();
    ASSERT_MSG(current_value.size() == 1, "field length must be 1");
    ASSERT_MSG(current_value.contains("field"), "json value must contain \"field\"");
    ASSERT_MSG(!current_value.at("field").is_double(), "got double value for field argument. Probably the value is too big to be represented as integer. You can put it in quotes to avoid JSON parser restrictions.");

    return parse_scalar<OperatingFieldType>(current_value.at("field"));
}

template<typename int_type>
int_type read_uint(boost::json::value& input_json_value, std::size_t position) {

    const boost::json::object &current_value = input_json_value.as_array()[position].as_object();
    ASSERT_MSG(current_value.size() == 1, "field length must be 1");
    ASSERT_MSG(current_value.contains("int"), "json value must contain \"int\"");
    ASSERT_MSG(!current_value.at("int").is_double(), "got double value for field argument. Probably the value is too big to be represented as integer. You can put it in quotes to avoid JSON parser restrictions.");

    int_type current_res;

    switch (current_value.at("int").kind()) {
        case boost::json::kind::int64:
            current_res = current_value.at("int").as_int64();
            break;
        case boost::json::kind::uint64:
            current_res = current_value.at("int").as_int64();
            break;
        default: {
            std::cerr << "wrong kind of input value: " << current_value.at("int").kind() << "\n";
            UNREACHABLE("wrong kind of input");
        }
    }

    return current_res;
}

template<typename PointType, typename FieldType>
PointType read_curve(boost::json::value& input_json_value, std::size_t position) {

    PointType res;

    const boost::json::object &current_value = input_json_value.as_array()[position].as_object();
    if(!current_value.contains("curve"))
        ASSERT(false && "json value must contain \"curve\"");
    if (!current_value.at("curve").is_array()) {
        ASSERT(false && "curve element must be array of length 2");
    }
    else {
        if(current_value.at("curve").as_array().size() != 2) {
            ASSERT(false && "curve element must be array of length 2");
        }
        else {
            if (current_value.at("curve").as_array()[0].is_double() ||
                current_value.at("curve").as_array()[1].is_double()) {
                ASSERT(false && "got double value for field argument. Probably the value is too big to be represented as integer. You can put it in quotes to avoid JSON parser restrictions.");
            }
            else {
                res.X = parse_scalar<FieldType>(current_value.at("curve").as_array()[0]);
                res.Y = parse_scalar<FieldType>(current_value.at("curve").as_array()[1]);
            }
        }
    }
    return res;
}

std::string read_string(boost::json::value& input_json_value, std::size_t position) {

    const boost::json::object &current_value = input_json_value.as_array()[position].as_object();
    if(!current_value.contains("string")) {
        std::cerr << "current_value: " << current_value << "\n";
        UNREACHABLE("json value must contain \"string\"");
    }
    if (!current_value.at("string").is_string()) {
        std::cerr << "current_value.at(\"string\"): " << current_value.at("string") << "\n";
        UNREACHABLE("value is not string.");
    }

    return current_value.at("string").as_string().c_str();
}

template<typename OperatingFieldType, std::size_t size>
std::array<typename OperatingFieldType::value_type, size>
    read_array_field(boost::json::value& input_json_value, std::size_t position) {

    std::array<typename OperatingFieldType::value_type, size> res;

    const boost::json::object &current_obj = input_json_value.as_array()[position].as_object();
    ASSERT(current_obj.size() == 1 && current_obj.contains("array"));
    ASSERT(current_obj.at("array").is_array());
    auto &arr = current_obj.at("array").as_array();

    for(std::size_t i = 0; i < size; i++) {
        ASSERT(arr[i].as_object().contains("field"));
        res[i] = parse_scalar<OperatingFieldType>(arr[i].as_object().at("field"));
    }

    return res;

}

template<typename OperatingFieldType, std::size_t size>
std::array<typename OperatingFieldType::value_type, size>
    read_vector_field(boost::json::value& input_json_value, std::size_t position) {

    std::array<typename OperatingFieldType::value_type, size> res;

    const boost::json::object &current_obj = input_json_value.as_array()[position].as_object();
    ASSERT(current_obj.size() == 1 && current_obj.contains("vector"));
    ASSERT(current_obj.at("vector").is_array());
    auto &arr = current_obj.at("vector").as_array();

    for(std::size_t i = 0; i < size; i++) {
        ASSERT(arr[i].as_object().contains("field"));
        res[i] = parse_scalar<OperatingFieldType>(arr[i].as_object().at("field"));
    }
    return res;
}


template<typename BlueprintFieldType, typename fp12_element>
fp12_element read_fp12(boost::json::value& input_json_value, std::size_t position) {

    std::array<typename BlueprintFieldType::value_type, 12> A;

    const boost::json::object &current_obj = input_json_value.as_array()[position].as_object();
    ASSERT(current_obj.size() == 1 && current_obj.contains("vector"));
    ASSERT(current_obj.at("vector").is_array());
    auto &arr = current_obj.at("vector").as_array();

    for(std::size_t i = 0; i < 12; i++) {
        ASSERT(arr[i].as_object().contains("field"));
        A[i] = parse_scalar<BlueprintFieldType>(arr[i].as_object().at("field"));
    }
    fp12_element res = fp12_element({ {A[0],A[1]}, {A[2],A[3]}, {A[4],A[5]} }, { {A[6],A[7]}, {A[8],A[9]}, {A[10],A[11]} });

    return res;
}
