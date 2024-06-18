# Hierarchical cell: emio_i2c_0
proc create_hier_cell_emio_i2c { parentCell nameHier } {

  variable script_folder

  set IIC_PORT_NUM 8
  if { $IIC_PORT_NUM > 32 && $IIC_PORT_NUM < 1 } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "IIC_PORT_NUM = <$IIC_PORT_NUM>. Expected to be <1~32>."}
     return
  }
  set EMIO_WIDTH [expr int(2*$IIC_PORT_NUM)]

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_emio_i2c() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  for { set a 0}  {$a < $IIC_PORT_NUM} {incr a} {
   create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_$a
  }



  # Create pins
  create_bd_pin -dir I -from [expr $EMIO_WIDTH - 1 ] -to 0 DinO
  create_bd_pin -dir I -from [expr $EMIO_WIDTH - 1 ] -to 0 DinT
  create_bd_pin -dir O -from [expr $EMIO_WIDTH - 1 ] -to 0 dout

  for { set a 0}  {$a < $IIC_PORT_NUM} {incr a} {
    # Create instance: xgpio_to_i2c_x, and set properties
    set xgpio_to_i2c_$a [ create_bd_cell -type ip -vlnv alinx.com.cn:user:xgpio_to_i2c:1.0 xgpio_to_i2c_$a ]
  }

  # Create instance: xlconcat_1, and set properties
  set xlconcat_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_1 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS $EMIO_WIDTH \
 ] $xlconcat_1

 for { set a 0}  {$a < $EMIO_WIDTH} {incr a} {
    # Create instance: xlslice_Ox, and set properties
    set xlslice_O$a [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O$a ]
    set_property -dict [ list \
    CONFIG.DIN_FROM $a \
    CONFIG.DIN_TO $a \
    CONFIG.DIN_WIDTH $EMIO_WIDTH \
    CONFIG.DOUT_WIDTH {1} \
    ] [set xlslice_O$a]
    # Create instance: xlslice_Tx, and set properties
    set xlslice_T$a [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T$a ]
    set_property -dict [ list \
    CONFIG.DIN_FROM $a \
    CONFIG.DIN_TO $a \
    CONFIG.DIN_WIDTH $EMIO_WIDTH \
    CONFIG.DOUT_WIDTH {1} \
    ] [set xlslice_T$a]
  }

  # Create interface connections
  for { set a 0}  {$a < $IIC_PORT_NUM} {incr a} {
    connect_bd_intf_net -intf_net xgpio_to_i2c_${a}_i2c_down [get_bd_intf_pins i2c_${a}] [get_bd_intf_pins xgpio_to_i2c_${a}/i2c_down]
  }

  # Create port connections
  #connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins DinO]
  #connect_bd_net -net axi_gpio_0_gpio_io_t [get_bd_pins DinT]
  for { set a 0}  {$a < $EMIO_WIDTH} {incr a} {
    connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins xlslice_O${a}/Din] [get_bd_pins DinO]
    connect_bd_net -net axi_gpio_0_gpio_io_t [get_bd_pins xlslice_T${a}/Din] [get_bd_pins DinT]
  }
  
  connect_bd_net -net xlconcat_1_dout [get_bd_pins dout] [get_bd_pins xlconcat_1/dout]

  for { set a 0}  {$a < $EMIO_WIDTH} {incr a} {
    set b [expr int($a / 2)]
    if { [expr $a % 2 ] == 0 } {
      connect_bd_net -net xgpio_to_i2c_0_upstream_sda_${b} [get_bd_pins xgpio_to_i2c_${b}/upstream_sda_O] [get_bd_pins xlconcat_1/In${a}]
      connect_bd_net -net xlslice_O${a}_Dout [get_bd_pins xgpio_to_i2c_${b}/upstream_sda_I] [get_bd_pins xlslice_O${a}/Dout]
      connect_bd_net -net xlslice_T${a}_Dout [get_bd_pins xgpio_to_i2c_${b}/upstream_sda_T] [get_bd_pins xlslice_T${a}/Dout]
    } else {
      connect_bd_net -net xgpio_to_i2c_0_upstream_scl_${b} [get_bd_pins xgpio_to_i2c_${b}/upstream_scl_O] [get_bd_pins xlconcat_1/In${a}]
      connect_bd_net -net xlslice_O${a}_Dout [get_bd_pins xgpio_to_i2c_${b}/upstream_scl_I] [get_bd_pins xlslice_O${a}/Dout]
      connect_bd_net -net xlslice_T${a}_Dout [get_bd_pins xgpio_to_i2c_${b}/upstream_scl_T] [get_bd_pins xlslice_T${a}/Dout]
    }
  }

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Create instance: emio_i2c_0
create_hier_cell_emio_i2c [current_bd_instance .] emio_i2c_0
