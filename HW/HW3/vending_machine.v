`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: ShanghaiTech Univ.
// Engineer: Zhaojun Ni
// 
// Create Date: 2023/11/09 20:58:22
// Design Name: 
// Module Name: vending_machine
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
module posedge_detect(
    input   CLK,
    input   RST,
    input   btn,	
	output  pos_edge   
);
    reg[1:0]   D;     
    always @(posedge CLK or negedge RST)begin
	    if(~RST)begin
            D <= 2'b00;
	    end else  begin
	        D <= {D[0],btn};  	
	    end
	end

	assign  pos_edge = ~D[1] & D[0];
endmodule

module vending_machine(
    input Enable,
    input RST,
    input CLK,
    input OneDollar,
    input FiftyCents,
    input TenCents,
    input FiveCents,
    output Deliver,
    output [7:0] Money
    );
parameter Price = 125;
reg [1:0] State,NextState;
reg [7:0] Sum;
parameter Idle = 2'd0;
parameter GoGo = 2'd1;
parameter Casher = 2'd2;
reg deliver_reg;
wire OneDollar_btn,FiftyCents_btn,TenCents_btn,FiveCents_btn;
wire OneDollar_db,FiftyCents_db,TenCents_db,FiveCents_db;
wire OneDollar_in,FiftyCents_in,TenCents_in,FiveCents_in;
assign Deliver = deliver_reg;
assign Money = Sum;
wire deliver_flag;
assign OneDollar_btn = (State)?OneDollar:0;
assign FiftyCents_btn = (State)?FiftyCents:0;
assign TenCents_btn = (State)?TenCents:0;
assign FiveCents_btn = (State)?FiveCents:0;
assign deliver_flag = (Sum>=Price)?1:0;
// Input with debouncing
debouncing  #(.N(3))in_100(
    .CLK(CLK),
    .ButtonIn(OneDollar_btn),
    .ButtonOut(OneDollar_db));

debouncing  #(.N(3))in_50(
    .CLK(CLK),
    .ButtonIn(FiftyCents_btn),
    .ButtonOut(FiftyCents_db));

debouncing  #(.N(3))in_10(
    .CLK(CLK),
    .ButtonIn(TenCents_btn),
    .ButtonOut(TenCents_db));

debouncing  #(.N(3))in_5(
    .CLK(CLK),
    .ButtonIn(FiveCents_btn),
    .ButtonOut(FiveCents_db));

// Posedge of input btns
posedge_detect detect_100(
    .CLK(CLK),
    .RST(RST),
    .btn(OneDollar_db),	
    .pos_edge(OneDollar_in));

posedge_detect detect_50(
    .CLK(CLK),
    .RST(RST),
    .btn(FiftyCents_db),	
    .pos_edge(FiftyCents_in));

posedge_detect detect_10(
    .CLK(CLK),
    .RST(RST),
    .btn(TenCents_db),	
    .pos_edge(TenCents_in));

posedge_detect detect_5(
    .CLK(CLK),
    .RST(RST),
    .btn(FiveCents_db),	
    .pos_edge(FiveCents_in));
    
// State Register
always@(posedge CLK or negedge RST)begin
    if (~RST) begin
        State <= Idle;
    end else begin
        State <= NextState;
    end
end

// NextState Logic 
always@(*)begin
    case (State) 
        Idle:
            if (Enable == 1'b1) begin
                NextState = GoGo;
            end else begin
                NextState = Idle;
            end
         GoGo:
            if (deliver_flag)begin
                NextState = Casher;
            end else begin
                NextState = GoGo;
            end
        Casher:
            NextState = Idle;
        default:
            NextState = Idle;
    endcase
end

// Output Logic
always @(*)begin
  case(State)
    Idle: begin 
        deliver_reg = 0;
        Sum = 0;
    end
    GoGo: deliver_reg = 0;
    Casher: deliver_reg = 1;
    default: deliver_reg = 0;
  endcase
end

// Money Logic
always @(posedge CLK or negedge RST)begin
    if (~RST) begin
        Sum <= 0;
    end else if (OneDollar_in) begin
        Sum <= Sum + 100;
    end else if (FiftyCents_in) begin
        Sum <= Sum + 50;
    end else if (TenCents_in) begin
        Sum <= Sum + 10;
    end else if (FiveCents_in) begin
        Sum <= Sum + 5;
    end else if (deliver_flag) begin
        Sum <= Sum - Price;
    end
end
endmodule
