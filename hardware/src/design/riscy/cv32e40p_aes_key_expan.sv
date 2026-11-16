module cv32e40p_aes_ks_unit
  import cv32e40p_pkg::*;
(
    input logic clk,
    input logic rst_n,
    input logic en_i,
    input logic[31:0] rs1_i,
    input logic[3:0] round_i,
    output logic[31:0] result_o
);
wire [7:0] v1, v2, v3, v4, w1, w2, w3, w4, w1_temp, w2_temp, w3_temp, w4_temp; 
reg [31:0] rcon;
wire [31:0] temp_rs1, result_temp, result_temp_swap;
assign temp_rs1 = {rs1_i[7:0],rs1_i[31:8]}; // SLR 8
assign v1 = temp_rs1[7:0];
assign v2 = temp_rs1[15:8];
assign v3 = temp_rs1[23:16]; 
assign v4 = temp_rs1[31:24];
cv32e40p_aes_sbox_fwd f1(.in_i(v1), .out_o(w1)); 
cv32e40p_aes_sbox_fwd f2(.in_i(v2), .out_o(w2)); 
cv32e40p_aes_sbox_fwd f3(.in_i(v3), .out_o(w3)); 
cv32e40p_aes_sbox_fwd f4(.in_i(v4), .out_o(w4));

always_comb begin
    case (round_i) 
      4'd1: rcon = 'h00000001;
      4'd2: rcon = 'h00000002; 
      4'd3: rcon = 'h00000004; 
      4'd4: rcon = 'h00000008; 
      4'd5: rcon = 'h00000010;
      4'd6: rcon = 'h00000020; 
      4'd7: rcon = 'h00000040; 
      4'd8: rcon = 'h00000080; 
      4'd9: rcon = 'h0000001B; 
      4'd10: rcon = 'h00000036;  
      default: rcon = 'h00; 
    endcase 
  end
  assign result_o = en_i? rcon ^ {w4, w3, w2, w1} : 'h00;     
endmodule
