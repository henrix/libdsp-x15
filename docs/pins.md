## BeagleBoard-X15 <-> CTAG face2|4 Pin Connections

| CTAG face2&#124;4 Pin Name | CTAG face2&#124;4 Pin No. | BB-X15 Pin No. | BB-X15 Pin Name | AM5728 Ball | Muxmode	| Offset | Signal	|
|:-------------------------- |:------------------------- |:-------------- |:--------------- |:----------- |:------------|:------ |:-------------|
| DLRCLK                     | 10           		 | P17.52 	  | MCASP2_FSX      | A18	  | 0		| 0x2F8	 | mcasp2_fsx	|
| DBCLK                      | 8            		 | P17.21 	  | MCASP2_ACLKX    | A19	  | 0		| 0x2F4	 | mcasp2_aclkx	|
| ALRCLK                     | 7            		 | P17.57 	  | MCASP2_FSR      | A20	  | 0		| 0x300	 | mcasp2_fsr	|
| ABCLK                      | 9            		 | P17.44 	  | MCASP2_ACLKR    | E15	  | 0		| 0x2FC	 | mcasp2_aclkr	|
| DSDATA1                    | 18           		 | P17.18 	  | MCASP2_AXR2     | C15	  | 0		| 0x30C	 | mcasp2_axr2	|
| ASDATA1                    | 3            		 | P17.54 	  | MCASP2_AXR0     | B15	  | 0		| 0x304	 | mcasp2_axr0	|
| spi4	                     | 17           		 | P16.3   	  | GPIO4_17	    | A12	  | 3		| 0x2E0	 | spi3_cs0	|
| spi3                       | 15           		 | P16.34   	  | GPIO5_10	    | B12	  | 3		| 0x2D4	 | spi3_sclk	|
| spi2                       | 13           		 | P16.33   	  | GPIO5_12	    | B13	  | 3		| 0x2DC	 | spi3_d0	|
| spi1                       | 11           		 | P16.4   	  | GPIO5_11	    | A11	  | 3		| 0x2D8	 | spi3_d1	|
| RESET_IN                   | 4            		 | P17.25   	  | GPIO5_9         | D12	  | 14		| 0x2D0	 | gpio5_9	|

For details about the expansion connectors check out the [BeagleBoard-X15 Schematic](https://github.com/beagleboard/beagleboard-x15/blob/master/BeagleBoard-X15_RevA2.pdf?raw=true)
and [AM5728 datasheet](http://www.ti.com/lit/ds/symlink/am5728.pdf).
