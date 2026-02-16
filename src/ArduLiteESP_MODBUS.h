#ifndef ARDULITEESP_MODBUS_H
#define ARDULITEESP_MODBUS_H

#include "ArduLiteESP_UART.h"

// Modbus Function Codes
#define MODBUS_FC_READ_COILS                0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS      0x02
#define MODBUS_FC_READ_HOLDING_REGISTERS    0x03
#define MODBUS_FC_READ_INPUT_REGISTERS      0x04
#define MODBUS_FC_WRITE_SINGLE_COIL         0x05
#define MODBUS_FC_WRITE_SINGLE_REGISTER     0x06
#define MODBUS_FC_WRITE_MULTIPLE_COILS      0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS  0x10

// Modbus Exception Codes
#define MODBUS_EX_ILLEGAL_FUNCTION          0x01
#define MODBUS_EX_ILLEGAL_DATA_ADDRESS      0x02
#define MODBUS_EX_ILLEGAL_DATA_VALUE        0x03
#define MODBUS_EX_SLAVE_DEVICE_FAILURE      0x04

// Modbus Settings
#define MODBUS_MAX_BUFFER                   256
#define MODBUS_DEFAULT_TIMEOUT              1000  // ms
#define MODBUS_FRAME_DELAY                  4     // ms (3.5 character times at 9600 baud)

class ModbusRTU {
protected:
    UART* uart_port;
    uint8_t device_address;
    uint32_t timeout_ms;
    
    uint8_t tx_buffer[MODBUS_MAX_BUFFER];
    uint8_t rx_buffer[MODBUS_MAX_BUFFER];
    uint16_t rx_length;
    uint32_t last_receive_time;
    
    // CRC16 calculation for Modbus
    uint16_t calculateCRC16(uint8_t* data, uint16_t length) {
        uint16_t crc = 0xFFFF;
        for (uint16_t i = 0; i < length; i++) {
            crc ^= (uint16_t)data[i];
            for (uint8_t j = 0; j < 8; j++) {
                if (crc & 0x0001) {
                    crc >>= 1;
                    crc ^= 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }
        return crc;
    }
    
    // Check CRC16
    bool checkCRC16(uint8_t* data, uint16_t length) {
        if (length < 3) return false;
        uint16_t received_crc = (uint16_t)data[length - 1] << 8 | data[length - 2];
        uint16_t calculated_crc = calculateCRC16(data, length - 2);
        return (received_crc == calculated_crc);
    }
    
    // Add CRC16 to buffer
    void addCRC16(uint8_t* data, uint16_t length) {
        uint16_t crc = calculateCRC16(data, length);
        data[length] = crc & 0xFF;
        data[length + 1] = (crc >> 8) & 0xFF;
    }
    
    // Send frame
    void sendFrame(uint8_t* frame, uint16_t length) {
        uart_port->flush();
        delay(MODBUS_FRAME_DELAY);
        
        for (uint16_t i = 0; i < length; i++) {
            uart_port->send(frame[i]);
        }
    }
    
    // Receive frame
    uint16_t receiveFrame(uint32_t timeout) {
        rx_length = 0;
        uint32_t start_time = millis();
        uint32_t last_char_time = millis();
        bool receiving = false;
        
        while (millis() - start_time < timeout) {
            if (uart_port->available()) {
                int data = uart_port->read();
                if (data >= 0) {
                    if (!receiving) {
                        receiving = true;
                        rx_length = 0;
                    }
                    
                    if (rx_length < MODBUS_MAX_BUFFER) {
                        rx_buffer[rx_length++] = (uint8_t)data;
                        last_char_time = millis();
                    }
                }
            }
            
            // Check for end of frame (3.5 character times of silence)
            if (receiving && (millis() - last_char_time > MODBUS_FRAME_DELAY)) {
                break;
            }
            
            delay(1);
        }
        
        return rx_length;
    }

public:
    ModbusRTU(UART* uart, uint8_t address = 1, uint32_t timeout = MODBUS_DEFAULT_TIMEOUT)
        : uart_port(uart), device_address(address), timeout_ms(timeout), rx_length(0), last_receive_time(0) {
    }
    
    void setAddress(uint8_t address) {
        device_address = address;
    }
    
    uint8_t getAddress() {
        return device_address;
    }
    
    void setTimeout(uint32_t timeout) {
        timeout_ms = timeout;
    }
};

// ==================== MODBUS MASTER ====================
class ModbusMaster : public ModbusRTU {
private:
    uint8_t last_exception;
    
    bool sendRequest(uint8_t slave_addr, uint8_t function_code, uint16_t start_addr, 
                     uint16_t quantity, uint8_t* data = nullptr, uint16_t data_length = 0) {
        uint16_t index = 0;
        
        tx_buffer[index++] = slave_addr;
        tx_buffer[index++] = function_code;
        tx_buffer[index++] = (start_addr >> 8) & 0xFF;
        tx_buffer[index++] = start_addr & 0xFF;
        
        if (function_code == MODBUS_FC_WRITE_SINGLE_COIL || 
            function_code == MODBUS_FC_WRITE_SINGLE_REGISTER) {
            tx_buffer[index++] = (quantity >> 8) & 0xFF;  // Value high byte
            tx_buffer[index++] = quantity & 0xFF;         // Value low byte
        } else if (function_code == MODBUS_FC_WRITE_MULTIPLE_COILS || 
                   function_code == MODBUS_FC_WRITE_MULTIPLE_REGISTERS) {
            tx_buffer[index++] = (quantity >> 8) & 0xFF;
            tx_buffer[index++] = quantity & 0xFF;
            tx_buffer[index++] = data_length;
            
            for (uint16_t i = 0; i < data_length; i++) {
                tx_buffer[index++] = data[i];
            }
        } else {
            tx_buffer[index++] = (quantity >> 8) & 0xFF;
            tx_buffer[index++] = quantity & 0xFF;
        }
        
        addCRC16(tx_buffer, index);
        index += 2;
        
        sendFrame(tx_buffer, index);
        return true;
    }
    
    bool waitResponse() {
        uint16_t length = receiveFrame(timeout_ms);
        
        if (length < 5) {
            return false;  // Too short
        }
        
        if (!checkCRC16(rx_buffer, length)) {
            return false;  // CRC error
        }
        
        // Check for exception response
        if (rx_buffer[1] & 0x80) {
            last_exception = rx_buffer[2];
            return false;
        }
        
        return true;
    }

public:
    ModbusMaster(UART* uart, uint32_t timeout = MODBUS_DEFAULT_TIMEOUT)
        : ModbusRTU(uart, 1, timeout), last_exception(0) {
    }
    
    uint8_t getLastException() {
        return last_exception;
    }
    
    // Read Coils (FC 0x01)
    bool readCoils(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity, bool* data) {
        if (quantity > 2000 || quantity == 0) return false;
        
        last_exception = 0;
        sendRequest(slave_addr, MODBUS_FC_READ_COILS, start_addr, quantity);
        
        if (!waitResponse()) return false;
        
        uint8_t byte_count = rx_buffer[2];
        for (uint16_t i = 0; i < quantity; i++) {
            uint16_t byte_index = i / 8;
            uint8_t bit_index = i % 8;
            data[i] = (rx_buffer[3 + byte_index] >> bit_index) & 0x01;
        }
        
        return true;
    }
    
    // Read Discrete Inputs (FC 0x02)
    bool readDiscreteInputs(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity, bool* data) {
        if (quantity > 2000 || quantity == 0) return false;
        
        last_exception = 0;
        sendRequest(slave_addr, MODBUS_FC_READ_DISCRETE_INPUTS, start_addr, quantity);
        
        if (!waitResponse()) return false;
        
        uint8_t byte_count = rx_buffer[2];
        for (uint16_t i = 0; i < quantity; i++) {
            uint16_t byte_index = i / 8;
            uint8_t bit_index = i % 8;
            data[i] = (rx_buffer[3 + byte_index] >> bit_index) & 0x01;
        }
        
        return true;
    }
    
    // Read Holding Registers (FC 0x03)
    bool readHoldingRegisters(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity, uint16_t* data) {
        if (quantity > 125 || quantity == 0) return false;
        
        last_exception = 0;
        sendRequest(slave_addr, MODBUS_FC_READ_HOLDING_REGISTERS, start_addr, quantity);
        
        if (!waitResponse()) return false;
        
        uint8_t byte_count = rx_buffer[2];
        for (uint16_t i = 0; i < quantity; i++) {
            data[i] = ((uint16_t)rx_buffer[3 + i * 2] << 8) | rx_buffer[4 + i * 2];
        }
        
        return true;
    }
    
    // Read Input Registers (FC 0x04)
    bool readInputRegisters(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity, uint16_t* data) {
        if (quantity > 125 || quantity == 0) return false;
        
        last_exception = 0;
        sendRequest(slave_addr, MODBUS_FC_READ_INPUT_REGISTERS, start_addr, quantity);
        
        if (!waitResponse()) return false;
        
        uint8_t byte_count = rx_buffer[2];
        for (uint16_t i = 0; i < quantity; i++) {
            data[i] = ((uint16_t)rx_buffer[3 + i * 2] << 8) | rx_buffer[4 + i * 2];
        }
        
        return true;
    }
    
    // Write Single Coil (FC 0x05)
    bool writeSingleCoil(uint8_t slave_addr, uint16_t addr, bool value) {
        last_exception = 0;
        uint16_t coil_value = value ? 0xFF00 : 0x0000;
        sendRequest(slave_addr, MODBUS_FC_WRITE_SINGLE_COIL, addr, coil_value);
        
        return waitResponse();
    }
    
    // Write Single Register (FC 0x06)
    bool writeSingleRegister(uint8_t slave_addr, uint16_t addr, uint16_t value) {
        last_exception = 0;
        sendRequest(slave_addr, MODBUS_FC_WRITE_SINGLE_REGISTER, addr, value);
        
        return waitResponse();
    }
    
    // Write Multiple Coils (FC 0x0F)
    bool writeMultipleCoils(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity, bool* data) {
        if (quantity > 1968 || quantity == 0) return false;
        
        last_exception = 0;
        uint8_t byte_count = (quantity + 7) / 8;
        uint8_t coil_data[246];  // Max bytes for coils
        
        memset(coil_data, 0, byte_count);
        
        for (uint16_t i = 0; i < quantity; i++) {
            if (data[i]) {
                uint16_t byte_index = i / 8;
                uint8_t bit_index = i % 8;
                coil_data[byte_index] |= (1 << bit_index);
            }
        }
        
        sendRequest(slave_addr, MODBUS_FC_WRITE_MULTIPLE_COILS, start_addr, quantity, 
                   coil_data, byte_count);
        
        return waitResponse();
    }
    
    // Write Multiple Registers (FC 0x10)
    bool writeMultipleRegisters(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity, uint16_t* data) {
        if (quantity > 123 || quantity == 0) return false;
        
        last_exception = 0;
        uint8_t byte_count = quantity * 2;
        uint8_t reg_data[246];  // Max bytes for registers
        
        for (uint16_t i = 0; i < quantity; i++) {
            reg_data[i * 2] = (data[i] >> 8) & 0xFF;
            reg_data[i * 2 + 1] = data[i] & 0xFF;
        }
        
        sendRequest(slave_addr, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, start_addr, quantity, 
                   reg_data, byte_count);
        
        return waitResponse();
    }
};

// ==================== MODBUS SLAVE ====================
class ModbusSlave : public ModbusRTU {
private:
    // Memory maps
    bool coils[256];
    bool discrete_inputs[256];
    uint16_t holding_registers[256];
    uint16_t input_registers[256];
    
    void sendException(uint8_t function_code, uint8_t exception_code) {
        tx_buffer[0] = device_address;
        tx_buffer[1] = function_code | 0x80;
        tx_buffer[2] = exception_code;
        addCRC16(tx_buffer, 3);
        sendFrame(tx_buffer, 5);
    }
    
    void sendResponse(uint16_t length) {
        addCRC16(tx_buffer, length);
        sendFrame(tx_buffer, length + 2);
    }
    
    void handleReadCoils(uint16_t start_addr, uint16_t quantity) {
        if (quantity == 0 || quantity > 2000) {
            sendException(MODBUS_FC_READ_COILS, MODBUS_EX_ILLEGAL_DATA_VALUE);
            return;
        }
        
        if (start_addr + quantity > 256) {
            sendException(MODBUS_FC_READ_COILS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        uint16_t index = 0;
        tx_buffer[index++] = device_address;
        tx_buffer[index++] = MODBUS_FC_READ_COILS;
        
        uint8_t byte_count = (quantity + 7) / 8;
        tx_buffer[index++] = byte_count;
        
        for (uint8_t i = 0; i < byte_count; i++) {
            uint8_t byte_value = 0;
            for (uint8_t bit = 0; bit < 8 && (i * 8 + bit) < quantity; bit++) {
                if (coils[start_addr + i * 8 + bit]) {
                    byte_value |= (1 << bit);
                }
            }
            tx_buffer[index++] = byte_value;
        }
        
        sendResponse(index);
    }
    
    void handleReadDiscreteInputs(uint16_t start_addr, uint16_t quantity) {
        if (quantity == 0 || quantity > 2000) {
            sendException(MODBUS_FC_READ_DISCRETE_INPUTS, MODBUS_EX_ILLEGAL_DATA_VALUE);
            return;
        }
        
        if (start_addr + quantity > 256) {
            sendException(MODBUS_FC_READ_DISCRETE_INPUTS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        uint16_t index = 0;
        tx_buffer[index++] = device_address;
        tx_buffer[index++] = MODBUS_FC_READ_DISCRETE_INPUTS;
        
        uint8_t byte_count = (quantity + 7) / 8;
        tx_buffer[index++] = byte_count;
        
        for (uint8_t i = 0; i < byte_count; i++) {
            uint8_t byte_value = 0;
            for (uint8_t bit = 0; bit < 8 && (i * 8 + bit) < quantity; bit++) {
                if (discrete_inputs[start_addr + i * 8 + bit]) {
                    byte_value |= (1 << bit);
                }
            }
            tx_buffer[index++] = byte_value;
        }
        
        sendResponse(index);
    }
    
    void handleReadHoldingRegisters(uint16_t start_addr, uint16_t quantity) {
        if (quantity == 0 || quantity > 125) {
            sendException(MODBUS_FC_READ_HOLDING_REGISTERS, MODBUS_EX_ILLEGAL_DATA_VALUE);
            return;
        }
        
        if (start_addr + quantity > 256) {
            sendException(MODBUS_FC_READ_HOLDING_REGISTERS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        uint16_t index = 0;
        tx_buffer[index++] = device_address;
        tx_buffer[index++] = MODBUS_FC_READ_HOLDING_REGISTERS;
        
        uint8_t byte_count = quantity * 2;
        tx_buffer[index++] = byte_count;
        
        for (uint16_t i = 0; i < quantity; i++) {
            tx_buffer[index++] = (holding_registers[start_addr + i] >> 8) & 0xFF;
            tx_buffer[index++] = holding_registers[start_addr + i] & 0xFF;
        }
        
        sendResponse(index);
    }
    
    void handleReadInputRegisters(uint16_t start_addr, uint16_t quantity) {
        if (quantity == 0 || quantity > 125) {
            sendException(MODBUS_FC_READ_INPUT_REGISTERS, MODBUS_EX_ILLEGAL_DATA_VALUE);
            return;
        }
        
        if (start_addr + quantity > 256) {
            sendException(MODBUS_FC_READ_INPUT_REGISTERS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        uint16_t index = 0;
        tx_buffer[index++] = device_address;
        tx_buffer[index++] = MODBUS_FC_READ_INPUT_REGISTERS;
        
        uint8_t byte_count = quantity * 2;
        tx_buffer[index++] = byte_count;
        
        for (uint16_t i = 0; i < quantity; i++) {
            tx_buffer[index++] = (input_registers[start_addr + i] >> 8) & 0xFF;
            tx_buffer[index++] = input_registers[start_addr + i] & 0xFF;
        }
        
        sendResponse(index);
    }
    
    void handleWriteSingleCoil(uint16_t addr, uint16_t value) {
        if (addr >= 256) {
            sendException(MODBUS_FC_WRITE_SINGLE_COIL, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        if (value != 0x0000 && value != 0xFF00) {
            sendException(MODBUS_FC_WRITE_SINGLE_COIL, MODBUS_EX_ILLEGAL_DATA_VALUE);
            return;
        }
        
        coils[addr] = (value == 0xFF00);
        
        // Echo request
        memcpy(tx_buffer, rx_buffer, 6);
        sendResponse(6);
    }
    
    void handleWriteSingleRegister(uint16_t addr, uint16_t value) {
        if (addr >= 256) {
            sendException(MODBUS_FC_WRITE_SINGLE_REGISTER, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        holding_registers[addr] = value;
        
        // Echo request
        memcpy(tx_buffer, rx_buffer, 6);
        sendResponse(6);
    }
    
    void handleWriteMultipleCoils(uint16_t start_addr, uint16_t quantity) {
        if (quantity == 0 || quantity > 1968) {
            sendException(MODBUS_FC_WRITE_MULTIPLE_COILS, MODBUS_EX_ILLEGAL_DATA_VALUE);
            return;
        }
        
        if (start_addr + quantity > 256) {
            sendException(MODBUS_FC_WRITE_MULTIPLE_COILS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        uint8_t byte_count = rx_buffer[6];
        
        for (uint16_t i = 0; i < quantity; i++) {
            uint16_t byte_index = i / 8;
            uint8_t bit_index = i % 8;
            coils[start_addr + i] = (rx_buffer[7 + byte_index] >> bit_index) & 0x01;
        }
        
        // Response
        uint16_t index = 0;
        tx_buffer[index++] = device_address;
        tx_buffer[index++] = MODBUS_FC_WRITE_MULTIPLE_COILS;
        tx_buffer[index++] = (start_addr >> 8) & 0xFF;
        tx_buffer[index++] = start_addr & 0xFF;
        tx_buffer[index++] = (quantity >> 8) & 0xFF;
        tx_buffer[index++] = quantity & 0xFF;
        
        sendResponse(index);
    }
    
    void handleWriteMultipleRegisters(uint16_t start_addr, uint16_t quantity) {
        if (quantity == 0 || quantity > 123) {
            sendException(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, MODBUS_EX_ILLEGAL_DATA_VALUE);
            return;
        }
        
        if (start_addr + quantity > 256) {
            sendException(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, MODBUS_EX_ILLEGAL_DATA_ADDRESS);
            return;
        }
        
        uint8_t byte_count = rx_buffer[6];
        
        for (uint16_t i = 0; i < quantity; i++) {
            holding_registers[start_addr + i] = ((uint16_t)rx_buffer[7 + i * 2] << 8) | 
                                                rx_buffer[8 + i * 2];
        }
        
        // Response
        uint16_t index = 0;
        tx_buffer[index++] = device_address;
        tx_buffer[index++] = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
        tx_buffer[index++] = (start_addr >> 8) & 0xFF;
        tx_buffer[index++] = start_addr & 0xFF;
        tx_buffer[index++] = (quantity >> 8) & 0xFF;
        tx_buffer[index++] = quantity & 0xFF;
        
        sendResponse(index);
    }

public:
    ModbusSlave(UART* uart, uint8_t address = 1)
        : ModbusRTU(uart, address, MODBUS_DEFAULT_TIMEOUT) {
        memset(coils, 0, sizeof(coils));
        memset(discrete_inputs, 0, sizeof(discrete_inputs));
        memset(holding_registers, 0, sizeof(holding_registers));
        memset(input_registers, 0, sizeof(input_registers));
    }
    
    // Set/Get memory values
    void setCoil(uint16_t addr, bool value) {
        if (addr < 256) coils[addr] = value;
    }
    
    bool getCoil(uint16_t addr) {
        return (addr < 256) ? coils[addr] : false;
    }
    
    void setDiscreteInput(uint16_t addr, bool value) {
        if (addr < 256) discrete_inputs[addr] = value;
    }
    
    bool getDiscreteInput(uint16_t addr) {
        return (addr < 256) ? discrete_inputs[addr] : false;
    }
    
    void setHoldingRegister(uint16_t addr, uint16_t value) {
        if (addr < 256) holding_registers[addr] = value;
    }
    
    uint16_t getHoldingRegister(uint16_t addr) {
        return (addr < 256) ? holding_registers[addr] : 0;
    }
    
    void setInputRegister(uint16_t addr, uint16_t value) {
        if (addr < 256) input_registers[addr] = value;
    }
    
    uint16_t getInputRegister(uint16_t addr) {
        return (addr < 256) ? input_registers[addr] : 0;
    }
    
    // Process incoming requests
    void process() {
        if (!uart_port->available()) return;
        
        uint16_t length = receiveFrame(100);
        
        if (length < 5) return;  // Too short
        
        // Check if this message is for us
        if (rx_buffer[0] != device_address && rx_buffer[0] != 0) return;
        
        // Check CRC
        if (!checkCRC16(rx_buffer, length)) return;
        
        uint8_t function_code = rx_buffer[1];
        uint16_t start_addr = ((uint16_t)rx_buffer[2] << 8) | rx_buffer[3];
        uint16_t quantity = ((uint16_t)rx_buffer[4] << 8) | rx_buffer[5];
        
        switch (function_code) {
            case MODBUS_FC_READ_COILS:
                handleReadCoils(start_addr, quantity);
                break;
                
            case MODBUS_FC_READ_DISCRETE_INPUTS:
                handleReadDiscreteInputs(start_addr, quantity);
                break;
                
            case MODBUS_FC_READ_HOLDING_REGISTERS:
                handleReadHoldingRegisters(start_addr, quantity);
                break;
                
            case MODBUS_FC_READ_INPUT_REGISTERS:
                handleReadInputRegisters(start_addr, quantity);
                break;
                
            case MODBUS_FC_WRITE_SINGLE_COIL:
                handleWriteSingleCoil(start_addr, quantity);
                break;
                
            case MODBUS_FC_WRITE_SINGLE_REGISTER:
                handleWriteSingleRegister(start_addr, quantity);
                break;
                
            case MODBUS_FC_WRITE_MULTIPLE_COILS:
                handleWriteMultipleCoils(start_addr, quantity);
                break;
                
            case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
                handleWriteMultipleRegisters(start_addr, quantity);
                break;
                
            default:
                sendException(function_code, MODBUS_EX_ILLEGAL_FUNCTION);
                break;
        }
    }
};

#endif
