#include "packet.hpp"

packet_data::packet_data(uint8_t* rxd)
{
    // m_date      = *rxd++;
    // m_time_hr   = *rxd++;
    // m_time_min  = *rxd++;
    m_lid       = (int) *rxd++;
    m_sid       = (int) *rxd++;

    uint8_t temp = *rxd++;
    m_length = temp & 0x7f;

    if (temp & 0x80)
    { // if you are here it is a float value, so obtain it as a char
        m_data_float = true;
        char* temp_char = new char(m_length);
        memcpy((void*) temp_char, (void*) rxd, m_length * sizeof(char));
        m_float_value = atof(temp_char);
        free(temp_char);
    }
    else
    {
        m_int_value = (int)*rxd;
        // TODO increase the data size;
    }
}