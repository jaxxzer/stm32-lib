//	       state = (state + 1) % 6; // 3 bits, represent phases
//
//	       uint8_t output = 0;
//
//	       switch (state) {
//	       case 0:
//	               output = 0b010;
//	               break;
//	       case 1:
//	               output = 0b011;
//	               break;
//	       case 2:
//	               output = 0b001;
//	               break;
//	       case 3:
//	               output = 0b101;
//	               break;
//	       case 4:
//	               output = 0b100;
//	               break;
//	       case 5:
//	               output = 0b110;
//	               break;
//	       default:
//	               break;
//	       }
//
//	       setPulse(&PA, output & (1 << 0) ? 1000 : 0);
//	       setPulse(&PB, output & (1 << 1) ? 1000 : 0);
//	       setPulse(&PC, output & (1 << 2) ? 1000 : 0);

class Brushless
{
public:
    Brushless(void);
    bool setRpmTarget(uint32_t rpm_target);
    uint32_t getRpmTarget(void);
    uint32_t getRpm(void);

private:
    void advanceCommutation(bool reverse=false);
    uint32_t _rpm;
    uint32_t _rpmTarget;
    volatile uint8_t state; // 3 bits


};