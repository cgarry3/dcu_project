// -------------------------------------------------------------------------
//       DCU Custom PWM
//
//  Author:       Cathal Garry
//  Description:  This is a custom PWM created in c++
//                using viviado HLS, it contains the following features
//                - No ap_ctrl port
//                - Two input register input ports
//                - one output register output port
//                - single output port containing PWM output pulse
// -------------------------------------------------------------------------

void dcu_pwm(volatile int run, volatile int delay, volatile int *count_pulse,volatile bool *pwm_out){
  // Removes valid signals on ports
  #pragma HLS INTERFACE ap_none register port=delay
  #pragma HLS INTERFACE ap_none register port=run
  #pragma HLS INTERFACE ap_none register port=count_pulse
  #pragma HLS INTERFACE ap_none register port=pwm_out

  // Removes ap_ctrl interface
  #pragma HLS INTERFACE ap_ctrl_none port=return


  volatile bool pwm_reg     = 0;
  volatile bool count_reg   = 0;

  while(run == 1) {
    volatile int i = 0;
    for (i = 0; i < delay; i++);
        	    *pwm_out     = !pwm_reg;
        	    *count_pulse = count_reg++;
  }
}
