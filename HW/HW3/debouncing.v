`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/11/09 20:59:44
// Design Name: 
// Module Name: debouncing
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
module debouncing#(parameter N=8)(
    input CLK,
    input ButtonIn,
    output reg ButtonOut
    );

reg [N-1:0] ButtonBuffer;

always@(posedge CLK)
begin
    ButtonBuffer <= {ButtonBuffer[N-2:0],ButtonIn};
    if (&ButtonBuffer == 1) begin
        ButtonOut <= 1;
    end else if (|ButtonBuffer == 0)begin
        ButtonOut <= 0;
    end
end
endmodule
