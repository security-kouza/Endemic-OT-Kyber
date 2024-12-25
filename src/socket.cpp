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

#include "socket.hpp"

#include <thread>
#include <chrono>

namespace ATLab {

    Socket::Socket(const std::string& address, const unsigned short port):
        _endpoint {boost::asio::ip::make_address(address), port},
        _socket {_ioc}
    {}

    void Socket::accept() {
        boost::asio::ip::tcp::acceptor{_ioc, _endpoint}.accept(_socket);
    }

    void Socket::connect(size_t MaxReconnectCnt) {
        size_t retryCnt {0};
        bool connected {false};
        constexpr auto baseDelayTime {std::chrono::milliseconds{5}};
        while (!connected) {
            try {
                _socket.connect(_endpoint);
                connected = true;
            } catch (const std::exception& e) {
                if (retryCnt >= MaxReconnectCnt) {
                    throw std::runtime_error("Failed to connect after maximum number of retries");
                }

                const auto delayTime {baseDelayTime * (1 << retryCnt)};
                std::this_thread::sleep_for(delayTime);
                ++retryCnt;
            }
        }
#ifdef DEBUG
        if (retryCnt) {
            std::cerr << "retried " << retryCnt << " times.\n";
        }
#endif // DEBUG
    }

    void Socket::close() {
        _socket.close();
    }

    size_t Socket::read(void* const pBuf, const size_t size, const std::string& logMsg, const bool debug) {
//        if (debug) {
//            print_log("Reading: " + logMsg);
//        }
        boost::asio::read(_socket, boost::asio::buffer(pBuf, size));
#ifdef DEBUG
        if (debug) {
            std::ostringstream sout;
            sout << readSize << " bytes read:\n";
            print_bytes(pBuf, readSize, sout);

            PrintMutex.lock();
            std::clog << sout.str();
            PrintMutex.unlock();
        }
#endif // DEBUG
        return 0;
    }

    size_t Socket::write(const void* pBuf, const size_t size, const std::string& logMsg, const bool debug) {
//        if (debug) {
//            print_log("Writing: " + logMsg);
//        }
        boost::asio::write(_socket, boost::asio::buffer(pBuf, size));
#ifdef DEBUG
        if (debug) {
            std::ostringstream sout;
            sout << writtenSize << " bytes written:\n";
            print_bytes(pBuf, writtenSize, sout);

            PrintMutex.lock();
            std::clog << sout.str();
            PrintMutex.unlock();
        }
#endif // DEBUG
        return 0;
    }
}
