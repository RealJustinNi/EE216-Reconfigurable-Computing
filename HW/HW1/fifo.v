`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: ShanghaiTech Univ.
// Engineer: Zhaojun Ni
// 
// Create Date: 2023/10/17 18:43:41
// Design Name: 
// Module Name: fifo
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////
module fifo(
    input clk,rst_n,
    input wr_en,rd_en,          // write enable signal and read enable signal
    input [7:0] buf_in,         // data input to be pushed to buffer
    output reg [7:0] buf_out,   // port to output the data using pop
    output empty, full,         // buffer empty and full indication
    output reg [3:0] fifo_cnt   // number of data pushed in to buffer
);
    reg [3:0] wp;
    reg [3:0] rp;
    
    reg [7:0] buffer [7:0];
    
    assign empty = (wp != rp)?1'b0:(fifo_cnt == 4'b0)?1'b1:1'b0;
    assign full  = (wp != rp)?1'b0:(fifo_cnt == 4'b0)?1'b0:1'b1;
   
    always @ (posedge clk or negedge rst_n) begin
        if(!rst_n) begin
            buf_out <= 8'b0;
            fifo_cnt<= 4'b0;
            wp      <= 3'b0;
            rp      <= 3'b0;
            buffer[0]  <= 8'b0;
            buffer[1]  <= 8'b0;
            buffer[2]  <= 8'b0;
            buffer[3]  <= 8'b0;
            buffer[4]  <= 8'b0;
            buffer[5]  <= 8'b0;
            buffer[6]  <= 8'b0;
            buffer[7]  <= 8'b0;
        end
        else begin 
            if(rd_en)begin
                buf_out    <= buffer[rp];
                rp         <= (rp==3'b111)?3'b000:rp+1;
            end
            if(wr_en) begin
                buffer[wp] <= buf_in;
                wp         <= (wp==3'b111)?3'b000:wp+1;
            end
            fifo_cnt  <= (~(rd_en ^ wr_en))?fifo_cnt:(rd_en)?fifo_cnt-1:fifo_cnt+1;
        end
    end
     
endmodule