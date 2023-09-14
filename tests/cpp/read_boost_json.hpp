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


boost::json::value read_boost_json (std::string input_file_name) {

    std::ifstream input_file(input_file_name.c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '" << input_file_name << "'" << std::endl;
        assert(false && "Could not open input file!");
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
        if (!input_json_value.as_array()[i].is_object()) {
            assert(false && "input must containt json objects");
        }
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
            assert(false && "json_value size exceeds buffer size");
        }
        json_value.as_string().copy(buf, numlen);
        buf[numlen] = '\0';
        integral_res = typename OperatingFieldType::extended_integral_type(buf);
        if (integral_res >= OperatingFieldType::modulus) {
            assert(false && "Input does not fit into OperatingFieldType");
        }
        res = integral_res;
        return res;
    }
    default:
        assert(false && "only integers and strings are supported");
    }
}

template<typename OperatingFieldType>
typename OperatingFieldType::value_type read_field(boost::json::value& input_json_value, std::size_t position) {

    const boost::json::object &current_value = input_json_value.as_array()[position].as_object();
    if (current_value.size() != 1)
        assert(false && "field length must be 1");
    if(!current_value.contains("field"))
        assert(false && "json value must contain \"field\"");
    if (current_value.at("field").is_double()) {
        assert(false && "got double value for field argument. Probably the value is too big to be represented as integer. You can put it in quotes to avoid JSON parser restrictions.");
    }

    return parse_scalar<OperatingFieldType>(current_value.at("field"));
}


uint32_t read_uint32_t(boost::json::value& input_json_value, std::size_t position) {

    const boost::json::object &current_value = input_json_value.as_array()[position].as_object();
    if (current_value.size() != 1)
        assert(false && "field length must be 1");
    if(!current_value.contains("int"))
        assert(false && "json value must contain \"int\"");
    if (current_value.at("int").is_double()) {
        assert(false && "got double value for field argument. Probably the value is too big to be represented as integer. You can put it in quotes to avoid JSON parser restrictions.");
    }

    uint32_t current_res;

    switch (current_value.at("int").kind()) {
        case boost::json::kind::int64:
            current_res = current_value.at("int").as_int64();
            break;
        case boost::json::kind::uint64:
            current_res = current_value.at("int").as_int64();
            break;
        default: {
            std::cerr << "wrong kind of input value: " << current_value.at("int").kind() << "\n";
            std::abort();
        }
    }

    return current_res;
}

template<typename PointType, typename FieldType>
PointType read_curve(boost::json::value& input_json_value, std::size_t position) {

    PointType res;

    const boost::json::object &current_value = input_json_value.as_array()[position].as_object();
    if(!current_value.contains("curve"))
        assert(false && "json value must contain \"curve\"");
    if (!current_value.at("curve").is_array()) {
        assert(false && "curve element must be array of length 2");
    }
    else {
        if(current_value.at("curve").as_array().size() != 2) {
            assert(false && "curve element must be array of length 2");
        }
        else {
            if (current_value.at("curve").as_array()[0].is_double() ||
                current_value.at("curve").as_array()[1].is_double()) {
                assert(false && "got double value for field argument. Probably the value is too big to be represented as integer. You can put it in quotes to avoid JSON parser restrictions.");
            }
            else {
                res.X = parse_scalar<FieldType>(current_value.at("curve").as_array()[0]);
                res.Y = parse_scalar<FieldType>(current_value.at("curve").as_array()[1]);
            }
        }
    }
    return res;
}