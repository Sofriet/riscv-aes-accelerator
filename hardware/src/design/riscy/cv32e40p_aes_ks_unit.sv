// Key-schedule unit for the custom instruction custom_keygen_f rd, rs1, round:
//   rd = SubWord(RotWord(rs1)) ^ Rcon[round], for round 1..10.
// Combinational; the result is zero when en_i is low.
module cv32e40p_aes_ks_unit (
    input  logic        en_i,
    input  logic [31:0] rs1_i,
    input  logic [ 3:0] round_i,
    output logic [31:0] result_o
);

  logic [31:0] rot_word;  // RotWord: rotate right by one byte (bytes are little-endian)
  logic [31:0] sub_word;  // SubWord: forward S-box on each byte
  logic [31:0] rcon;

  assign rot_word = {rs1_i[7:0], rs1_i[31:8]};

  for (genvar i = 0; i < 4; i++) begin : gen_sbox
    cv32e40p_aes_sbox_fwd sbox_i (
        .in_i (rot_word[8*i+:8]),
        .out_o(sub_word[8*i+:8])
    );
  end

  always_comb begin
    case (round_i)
      4'd1:    rcon = 32'h01;
      4'd2:    rcon = 32'h02;
      4'd3:    rcon = 32'h04;
      4'd4:    rcon = 32'h08;
      4'd5:    rcon = 32'h10;
      4'd6:    rcon = 32'h20;
      4'd7:    rcon = 32'h40;
      4'd8:    rcon = 32'h80;
      4'd9:    rcon = 32'h1B;
      4'd10:   rcon = 32'h36;
      default: rcon = 32'h00;
    endcase
  end

  assign result_o = en_i ? (sub_word ^ rcon) : 32'h0;

endmodule
