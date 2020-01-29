#ifndef SETR_IO_EVENT_H
#define SETR_IO_EVENT_H

enum io_event
{
    IO_0_EVENT,IO_1_EVENT,IO_2_EVENT,IO_3_EVENT,IO_4_EVENT,IO_5_EVENT,
    IO_6_EVENT,IO_7_EVENT,IO_8_EVENT,IO_9_EVENT,IO_10_EVENT,IO_11_EVENT,
    IO_12_EVENT,IO_13_EVENT,IO_14_EVENT,IO_15_EVENT,IO_16_EVENT,IO_17_EVENT,
    IO_18_EVENT,IO_19_EVENT,IO_20_EVENT,IO_21_EVENT,IO_22_EVENT,IO_23_EVENT,
    IO_24_EVENT,IO_25_EVENT,IO_26_EVENT,IO_27_EVENT,IO_28_EVENT,IO_29_EVENT,
    IO_30_EVENT,IO_31_EVENT,IO_32_EVENT,IO_33_EVENT,IO_34_EVENT,IO_35_EVENT,
    IO_36_EVENT,IO_37_EVENT,IO_38_EVENT,IO_39_EVENT
};

namespace std
{
    template <>
    struct hash<io_event>
    {
        std::size_t operator()(const io_event& v) const
        {
            return static_cast<size_t>(v);
        }
    };
}

#endif //SETR_IO_EVENT_H
