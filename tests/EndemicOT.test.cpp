/*
This file is part of EOTKyber of Abe-Tibouchi Laboratory, Kyoto University
Copyright (C) 2023-2024  Kyoto University
Copyright (C) 2023-2024  Peihao Li <li.peihao.62s@st.kyoto-u.ac.jp>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <vector>
#include <random>
#include <bitset>
#include <array>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <limits>

#include <gtest/gtest.h>

#include "EndemicOT/EndemicOT.hpp"
#include "PRNG.hpp"

namespace {
    std::ostream& operator<<(std::ostream& out, const ATLab::EndemicOT::DataBlock& data) {
        std::ios store {nullptr};
        store.copyfmt(out);

        out << "0x" << std::hex << std::setfill('0');
        for (const auto& byte : data) {
            out << std::setw(2) << static_cast<int>(byte);
        }

        out.copyfmt(store);
        return out;
    }

    std::mt19937_64 rng; // NOLINT(*-msc51-cpp) // intended fixed seed for debug use
//    ATLab::PRNG_Kyber rng{ATLab::PRNG_Kyber::get_PRNG_Kyber()};

    template<class T, size_t N>
    void write_random_data(std::array<T,N>& buf) {
        std::uniform_int_distribution<uint64_t> rDist(0, 0xff);
        for (size_t i{0}; i != N * sizeof(T); ++i) {
            *(reinterpret_cast<uint8_t*>(&buf) + i) = rDist(rng);
        }
    }
}

TEST(EndemicOT, UnitTest) {
    using namespace ATLab;

    constexpr size_t NUM_OT {128};

    std::uniform_int_distribution<uint64_t> rDist(0, std::numeric_limits<uint64_t>::max());
    std::bitset<64> choices {rDist(rng)};

    std::vector<EndemicOT::Sender> senders;
    std::vector<std::array<EndemicOT::Sender::Data, 2>> dataToSend;

    std::vector<EndemicOT::Receiver> receivers;
    std::vector<EndemicOT::Sender::Data> receivedData;

    // prepare Data
    for (size_t i = 0; i < NUM_OT; ++i) {
        receivers.emplace_back(choices[i % 64]);
        dataToSend.emplace_back();
        write_random_data(dataToSend.at(i).at(0));
        write_random_data(dataToSend.at(i).at(1));
        senders.emplace_back(dataToSend.at(i).at(0), dataToSend.at(i).at(1));
    }

    // OT
    const auto start {std::chrono::high_resolution_clock::now()};
    for (size_t i = 0; i < NUM_OT; ++i) {
        auto rMsg {receivers.at(i).get_receiver_msg()};
        auto sMsg {senders.at(i).encrypt_with(rMsg)};
        receivedData.emplace_back(receivers.at(i).decrypt_chosen(sMsg));
    }
    const auto end {std::chrono::high_resolution_clock::now()};

    const std::chrono::duration<double, std::milli> duration {end - start};
    std::cout << std::setprecision(3) << duration.count() << "ms for " << NUM_OT << " OTs "
        "(" << std::setprecision(3) << duration.count() * 1000 / NUM_OT << "μs per OT)\n";

    // check
    for (size_t i = 0; i < NUM_OT; ++i) {
        if (receivedData.at(i) != dataToSend.at(i).at(choices[i % 64])) {
            std::stringstream strFormatter;
            strFormatter << "failed " << i << " exp = m[" << choices[i % 64] << "],"
                "act = " << receivedData.at(i)
                << " true = " << dataToSend.at(i).at(0)
                << ", " << dataToSend.at(i).at(1) << '\n';
            std::string errorMsg;
            std::getline(strFormatter, errorMsg);
            throw std::runtime_error{errorMsg};
        }
    }
}

const std::string IP {"127.0.0.1"};
constexpr int PORT {12345}; // TODO: check if port occupied

TEST(EndemicOT, BatchTest) {
    constexpr size_t NUM_OT {128};
    std::array<__m128i, NUM_OT> data0 {}, data1 {}, data {};
    std::array<bool, NUM_OT> choices {};

    std::chrono::duration<double> durationSender {0}, durationReceiver {0};

    constexpr size_t REPEAT_CNT {1000};

    std::thread sender {
        [&data0, &data1, &durationSender]() {
            ATLab::Socket socket(IP, PORT);
            socket.connect();

            std::byte syncByte {0};
            socket.write(&syncByte, 1);

            const auto start {std::chrono::high_resolution_clock::now()};
            for (size_t i = 0; i != REPEAT_CNT; ++i) {
                write_random_data(data0);
                write_random_data(data1);
                ATLab::EndemicOT::batch_send(socket, data0.data(), data1.data(), data0.size());
            }
            const auto end {std::chrono::high_resolution_clock::now()};
            durationSender = end - start;
        }
    }, receiver {
        [&data, &choices, &durationReceiver]() {
            ATLab::Socket socket(IP, PORT);
            socket.accept();

            std::byte syncByte {0};
            socket.read(&syncByte, 1);

            const auto start {std::chrono::high_resolution_clock::now()};
            for (size_t i = 0; i != REPEAT_CNT; ++i) {
                choices = ATLab::random_bool_array<NUM_OT>();
                ATLab::EndemicOT::batch_receive(socket, data.data(), choices.data(), data.size());
            }
            const auto end {std::chrono::high_resolution_clock::now()};
            durationReceiver = end - start;
        }
    };

    sender.join();
    receiver.join();

    durationSender /= REPEAT_CNT;
    durationReceiver /= REPEAT_CNT;

    std::cout << "Sender  : " << std::setprecision(4) << durationSender.count() << "s for " << NUM_OT << " OTs "
        "(" << std::setprecision(3) << durationSender.count() / NUM_OT * 1000000 << "μs per OT)\n";

    std::cout << "Receiver: " << std::setprecision(4) << durationReceiver.count() << "s for " << NUM_OT << " OTs "
        "(" << std::setprecision(3) << durationReceiver.count() / NUM_OT * 1000000 << "μs per OT)\n";

    auto& d0 = *reinterpret_cast<const std::array<__uint128_t, 128>*>(&data0);
    auto& d1 = *reinterpret_cast<const std::array<__uint128_t, 128>*>(&data1);
    auto& d = *reinterpret_cast<const std::array<__uint128_t, 128>*>(&data);

    // REPEAT_CNT only to calculate the average time
    // Thus here only checking the correctness of one (which is the last) batched OT
    for (size_t i {0}; i != 128; ++i) {
        EXPECT_EQ(d.at(i), choices.at(i) ? d1.at(i) : d0.at(i));
    }
}
