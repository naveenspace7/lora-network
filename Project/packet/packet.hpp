#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// packet structure:
// | date | time_hr | time_min | l-id | s-id | float_flag & length | value |
// | l-id | s-id | float_flag & length | value |

class packet_data
{
    public:
        bool m_data_float = false;
        int m_lid, m_sid;
        float m_float_value;
        int m_int_value;
        packet_data(uint8_t*);
    private:
        size_t m_length;
};