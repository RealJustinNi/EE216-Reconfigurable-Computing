`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: ShanghaiTech Univ.
// Engineer: Zhaojun Ni
// 
// Create Date: 2023/11/05 12:20:48
// Design Name: 
// Module Name: myFIR_tb
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


module myFIR_tb();
reg clk;
reg rst_n;
reg S_AXIS_TVALID;
reg S_AXIS_TLAST;
reg [7:0] S_AXIS_TDATA;
reg  M_AXIS_TREADY;
wire S_AXIS_TREADY;
wire M_AXIS_TVALID;
wire M_AXIS_TLAST;
wire [31:0] M_AXIS_TDATA;

myFIR myFIR(
    .clk(clk),
    .rst_n(rst_n),
    .S_AXIS_TVALID(S_AXIS_TVALID),
    .S_AXIS_TLAST(S_AXIS_TLAST),
    .M_AXIS_TREADY(M_AXIS_TREADY),
    .S_AXIS_TREADY(S_AXIS_TREADY),
    .M_AXIS_TVALID(M_AXIS_TVALID),
    .M_AXIS_TLAST(M_AXIS_TLAST),
    .S_AXIS_TDATA(S_AXIS_TDATA),
    .M_AXIS_TDATA(M_AXIS_TDATA)
);

always #5 clk=~clk;

initial begin
    clk <= 0;
    rst_n <= 0;
    S_AXIS_TVALID <= 0;
    S_AXIS_TLAST <= 0;
    S_AXIS_TDATA <= 0;
    M_AXIS_TREADY <= 0;
    #50;
    rst_n <= 1;
    S_AXIS_TVALID <= 1;
    S_AXIS_TLAST <= 1;
    S_AXIS_TDATA <= 1;
    M_AXIS_TREADY <= 1;
    #10;
    S_AXIS_TDATA <= 0;
    S_AXIS_TVALID <= 0;
    S_AXIS_TLAST <= 0;
    M_AXIS_TREADY <= 0;
    #50;
end
endmodule
