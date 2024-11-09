library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity decoder_8to256 is
    Port (
        input  : in  STD_LOGIC_VECTOR(7 downto 0); -- 8-bit input
        output : out STD_LOGIC_VECTOR(255 downto 0) -- 256-bit output
    );
end decoder_8to256;

architecture Behavioral of decoder_8to256 is
begin
    process(input)
    begin
        -- Initialize output to 0
        output <= (others => '0');

        -- Set only one output bit high based on the input value
        output(to_integer(unsigned(input))) <= '1';
    end process;
end Behavioral;
