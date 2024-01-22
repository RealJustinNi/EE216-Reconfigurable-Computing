`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: ShanghaiTech Univ.
// Engineer: Zhaojun Ni
// 
// Create Date: 2023/11/05 12:20:24
// Design Name: 
// Module Name: myFIR
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

module myFIR(
    input clk,
    input rst_n,
    input S_AXIS_TVALID,
    input S_AXIS_TLAST,
    input [7:0] S_AXIS_TDATA,
    output S_AXIS_TREADY,
    input M_AXIS_TREADY,
    output M_AXIS_TVALID,
    output M_AXIS_TLAST,
    output [31:0] M_AXIS_TDATA
);

parameter signed [4:0] h0 = 5'd1;
parameter signed [4:0] h1 = -5'd2;
parameter signed [4:0] h2 = 5'd3;
parameter signed [4:0] h3 = -5'd4;
parameter signed [4:0] h4 = 5'd5;
parameter signed [4:0] h5 = -5'd6;
parameter signed [4:0] h6 = 5'd7;
parameter signed [4:0] h7 = -5'd8;

reg [31:0] maxis_tdata;
reg maxis_tvalid, maxis_tlast, saxis_tready;
assign M_AXIS_TDATA = maxis_tdata;
assign M_AXIS_TVALID = maxis_tvalid;
assign M_AXIS_TLAST = maxis_tlast;
assign S_AXIS_TREADY = saxis_tready;

reg tlast_delay;
reg fir_ready;
reg adder_ready;
wire adder_valid;
wire adder_ready_w;
reg signed [7:0] data_tmp [15:0];
reg signed [13:0] data_mult[7:0];
wire [14*8-1:0] adder_input;
reg  [4:0] cnt;
wire signed [16:0] sumout;
assign adder_ready_w = adder_ready;
// Pipeline1: Load data from AXI to data_tmp[15:0]
generate
    genvar j;
    for (j = 1; j <= 15; j = j + 1)
        begin: shift_data_tmp
            always@(posedge clk or negedge rst_n)begin
                if(~rst_n)begin
                    data_tmp[j] <= 0;
                end
                else if(fir_ready)begin
                    data_tmp[j] <= data_tmp[j-1];
                end
            end
        end
endgenerate

// Pipeline2: Finish data_mult = (data_tmp_a + data_tmp_b)* h
always@(posedge clk or negedge rst_n)begin
    if(~rst_n)begin
        data_mult[0] <= 0;
        data_mult[1] <= 0;
        data_mult[2] <= 0;
        data_mult[3] <= 0;
        data_mult[4] <= 0;
        data_mult[5] <= 0;
        data_mult[6] <= 0;
        data_mult[7] <= 0; 
        adder_ready  <= 0;
    end 
    else if(fir_ready)begin
        data_mult[0] <= (data_tmp[0]+data_tmp[15])*h0;
        data_mult[1] <= (data_tmp[1]+data_tmp[14])*h1;
        data_mult[2] <= (data_tmp[2]+data_tmp[13])*h2;
        data_mult[3] <= (data_tmp[3]+data_tmp[12])*h3;
        data_mult[4] <= (data_tmp[4]+data_tmp[11])*h4;
        data_mult[5] <= (data_tmp[5]+data_tmp[10])*h5;
        data_mult[6] <= (data_tmp[6]+data_tmp[9])*h6;
        data_mult[7] <= (data_tmp[7]+data_tmp[8])*h7;
        adder_ready <= 1;
    end
end

assign adder_input = {data_mult[7],data_mult[6],data_mult[5],data_mult[4],data_mult[3],data_mult[2],data_mult[1],data_mult[0]};
// Pipeline 3-5 Multicycle Adder
MultiCycleAdder myadder(
    .clk(clk),
    .rst_n(rst_n),
    .din(adder_input),
    .enable(adder_ready_w),
    .valid(adder_valid),
    .dout(sumout)
);

// AXI interface
always@(posedge clk or negedge rst_n) begin
    if (~rst_n) begin
        maxis_tvalid <= 0;
        saxis_tready <= 0;
        maxis_tdata <= 0;
        maxis_tlast <= 0;
        tlast_delay <= 0;
        fir_ready <= 0;
        data_tmp[0] <= 0;
        cnt <= 0;
    end
    else begin
        data_tmp[0] <= 0;
        saxis_tready <= 1;
        tlast_delay <= S_AXIS_TLAST;
        
        // Cycle Counter from handshake started 
        if (fir_ready) begin
            cnt <= cnt + 1;
        end
        else begin
            cnt <= cnt;
        end

        // S_VALID M_READY Handshake Success
        if (S_AXIS_TVALID  && M_AXIS_TREADY && (~tlast_delay)) begin
            data_tmp[0] <= S_AXIS_TDATA;
            fir_ready <= 1;
            cnt <= 0;
        end
        
        // Not Handshake  while connected
        else if (fir_ready) begin
            if (cnt < 5'd4) begin
                fir_ready <= 1;
                maxis_tvalid <= 0;
                maxis_tdata <= 0;
            end 
            else if (cnt <= 5'd19) begin
                fir_ready <= 1;
                maxis_tvalid <= 1; 
                maxis_tdata <= sumout;
                //maxis_tdata <= cnt;
            end else begin
                fir_ready <= 0;
                maxis_tvalid <= 0;
                maxis_tdata <= 0;
            end
        end 
        else begin
            maxis_tvalid <= 0;
            maxis_tdata <= 0;
        end
        
        // M_Last
        if (cnt == 5'd19) begin
            maxis_tlast <= 1;
        end
        else begin
            maxis_tlast <= 0;
        end
    end
end

endmodule
