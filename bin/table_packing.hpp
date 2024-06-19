std::string add_filename_prefix(
    const std::string& prefix,
    const std::string& file_name
) {
    std::filesystem::path path(file_name);
    std::filesystem::path parent_path = path.parent_path();
    std::filesystem::path filename = path.filename();

    std::string new_filename = prefix + filename.string();
    std::filesystem::path new_path = parent_path / new_filename;

    return new_path.string();
}


bool append_binary_file_content_to_vector (
    std::vector<std::uint8_t>& result_vector,
    const std::string& prefix,
    const std::string& assignment_table_file_name

) {
    std::ifstream icolumn;
    icolumn.open(add_filename_prefix(prefix, assignment_table_file_name), std::ios_base::binary | std::ios_base::in);
    if (!icolumn) {
        std::cout << "Cannot open " << add_filename_prefix(prefix, assignment_table_file_name) << std::endl;
        return false;
    }
    icolumn.seekg(0, std::ios_base::end);
    const auto input_size = icolumn.tellg();
    std::size_t old_size = result_vector.size();
    result_vector.resize(old_size + input_size);
    icolumn.seekg(0, std::ios_base::beg);
    icolumn.read(reinterpret_cast<char*>(result_vector.data() + old_size), input_size);
    icolumn.close();
    return true;
}

template<typename marshalling_type>
void unmarshall_from_vector (
    const std::vector<std::uint8_t>& v,
    marshalling_type& marhsalling_data
) {
    auto vect_iterator = v.begin();
    auto marshalling_status = marhsalling_data.read(vect_iterator, v.size());
    ASSERT(marshalling_status == nil::marshalling::status_type::success);
}

template<typename marshalling_type>
void extract_from_binary_file(
    marshalling_type& marshalling_file,
    const std::string& prefix,
    const std::string& file_name
) {
    std::vector<std::uint8_t> file_vector = {};
    ASSERT(append_binary_file_content_to_vector(file_vector, prefix, file_name));
    unmarshall_from_vector<marshalling_type> (file_vector, marshalling_file);
}

template<typename marshalling_type>
marshalling_type extract_table_from_binary_file(
    const std::string& file_name
) {
    std::vector<std::uint8_t> file_vector = {};
    ASSERT(append_binary_file_content_to_vector(file_vector, "header_", file_name));
    ASSERT(append_binary_file_content_to_vector(file_vector, "witness_", file_name));
    ASSERT(append_binary_file_content_to_vector(file_vector, "pub_inp_", file_name));
    ASSERT(append_binary_file_content_to_vector(file_vector, "constants_", file_name));
    ASSERT(append_binary_file_content_to_vector(file_vector, "selectors_", file_name));
    marshalling_type marshalling_data;
    unmarshall_from_vector<marshalling_type> (file_vector, marshalling_data);
    return marshalling_data;
}
