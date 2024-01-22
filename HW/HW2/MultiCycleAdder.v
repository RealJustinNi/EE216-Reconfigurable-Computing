`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Shanghaitech Univ.
// Engineer: Zhaojun Ni
// 
// Create Date: 2023/11/05 12:23:34
// Design Name: 
// Module Name: MultiCycleAdder
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


module MultiCycleAdder#(parameter INPUT_WIDTH = 4'd14, OUTPUT_WIDTH = 5'd17)(
    input clk,
    input rst_n,
    input signed [INPUT_WIDTH*8-1:0] din,
    input enable,
    output valid,
    output [OUTPUT_WIDTH-1:0] dout
    );

reg signed [INPUT_WIDTH:0] sum1 [3:0];
reg signed [INPUT_WIDTH+1:0] sum2 [1:0];
reg signed [INPUT_WIDTH+2:0] sum3;
reg L1_valid,L2_valid,L3_valid;
assign valid = L3_valid & enable;   
assign dout = sum3;

// Pipeline_Level1
generate
    genvar i;
    for (i = 0; i <= 3; i = i + 1)
        begin: eight_four
            always@(posedge clk or negedge rst_n)begin
                if(~rst_n)begin
                   sum1[i] <= 0;
                   L1_valid <= 0;
                end
                else if(enable)begin
                    sum1[i] <= $signed(din[INPUT_WIDTH*(2*i+2)-1:INPUT_WIDTH*(2*i+1)]) + $signed(din[INPUT_WIDTH*(2*i+1)-1:INPUT_WIDTH*2*i]); 
                    L1_valid <= 1;                  
                end
            end
        end
endgenerate

// Pipeline_Level2
always@(posedge clk or negedge rst_n)begin
    if(~rst_n)begin
       sum2[0] <= 0;
       sum2[1] <= 0;
       L2_valid <= 0;
    end
    else if(L1_valid & enable)begin
        sum2[0] <= sum1[0]+sum1[1];         
        sum2[1] <= sum1[2]+sum1[3];  
        L2_valid <= 1;              
    end
end

// Pipeline_Level3
always@(posedge clk or negedge rst_n)begin
    if(~rst_n)begin
       sum3 <= 0;
       L3_valid <= 0;
    end
    else if(L2_valid & enable)begin
        sum3 <= sum2[0]+sum2[1];         
        L3_valid <= 1;               
    end
end
endmodule
