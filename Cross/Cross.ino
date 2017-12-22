#include <Adafruit_NeoPixel.h>

// Pattern types:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, SCANNER_CHASE, COLOR_WIPE, SCANNER, FADE, FIREWORKS, ALL_WHITE, ALL_RED, DIRECTIONAL_SCANNER, FIRE, WATER };
// Patern directions:
enum  direction { FORWARD, REVERSE };


void SmallComplete();
void LargeComplete();


// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
	public:
	// Member Variables:  
	pattern ActivePattern;		// which pattern is running
	direction Direction;		// direction to run the pattern

	unsigned long Interval;		// milliseconds between updates
	unsigned long lastUpdate;	// last update of position

	uint32_t Color1, Color2;	// What colors are in use
	uint16_t TotalSteps;		// total number of steps in the pattern
	uint16_t Index;				// current step within the pattern

	void (*OnComplete)();		// Callback on completion of pattern

	// Constructor - calls base-class constructor to initialize Sides
	NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
	:Adafruit_NeoPixel(pixels, pin, type)
	{
		OnComplete = callback;
	}

	// Update the pattern
	void Update()
	{
		if((millis() - lastUpdate) > Interval) // time to update
		{
			lastUpdate = millis();
			switch(ActivePattern)
			{
				case FIRE:
					FireUpdate();
					break;
				case WATER:
					WaterUpdate();
					break;
				case RAINBOW_CYCLE:
					RainbowCycleUpdate();
					break;
				case THEATER_CHASE:
					TheaterChaseUpdate();
					break;
				case SCANNER_CHASE:
					ScannerChaseUpdate();
					break;
				case COLOR_WIPE:
					ColorWipeUpdate();
					break;
				case SCANNER:
					ScannerUpdate();
					break;
				case FIREWORKS:
					FireWorksUpdate();
					break;
				case DIRECTIONAL_SCANNER:
					DirectionalScannerUpdate();
					break;
				case FADE:
					FadeUpdate();
					break;
				default:
					break;
			}
		}
	}
	
	// Increment the Index and reset at the end
	void Increment()
	{
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else //(Direction == REVERSE)
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
	}
    
	// Reverse pattern direction
	void Reverse()
	{
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            Index = 0;
        }
	}
    
	// ------- vv Fire pattern vv -------
	void Fire(uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = FIRE;
        Interval = interval;
        TotalSteps = numPixels();
        Index = 0;
        Direction = dir;
	}
    
	// Update the Rainbow Cycle Pattern
	void FireUpdate()
	{
        for(int i=0; i< numPixels(); i++)
        {
          int FireR = 255;
          int FireG = FireR-100;
          int FireB = 75;
          int flicker = random(0,150);
          int FireR1 = FireR-flicker;
          int FireG1 = FireG-flicker;
          int FireB1 = FireB-flicker;
          if(FireR1<0) FireR1=0;
          if(FireG1<0) FireG1=0;
          if(FireB1<0) FireB1=0;
          setPixelColor(i, FireR1, FireG1, FireB1);
        }
        show();
        Increment();
	}
	// ------- ^^ Fire pattern ^^ -------
	
	
	// ------- vv Water pattern vv -------
	void Water(uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = WATER;
        Interval = interval;
        TotalSteps = numPixels();
        Index = 0;
        Direction = dir;
	}

	void WaterUpdate()
	{
        for(int i=0; i< numPixels(); i++)
        {
          int WaterR = random(30,80);
          int WaterB = random(200,255);
          int WaterG = random(100,150);
          int flicker = random(0,100);
          int WaterR1 = WaterR-flicker;
          int WaterG1 = WaterG-flicker;
          int WaterB1 = WaterB-flicker;
          if(WaterR1<0) WaterR1=0;
          if(WaterG1<0) WaterG1=0;
          if(WaterB1<0) WaterB1=0;
          setPixelColor(i, WaterR1, WaterG1, WaterB1);
        }
        show();
        Increment();
	}
	// ------- ^^ Water pattern ^^ -------
	
	// ------- vv RainbowCycle pattern vv -------
	void RainbowCycle(uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;
	}

	void RainbowCycleUpdate()
	{
        for(int i=0; i< numPixels(); i++)
        {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        show();
        Increment();
	}
	// ------- ^^ RainbowCycle pattern ^^ -------
    
	// ------- vv TheaterChase pattern vv -------
	void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
	}

	void TheaterChaseUpdate()
	{
        for(int i=0; i< numPixels(); i++)
        {
            if ((i + Index) % 3 == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                setPixelColor(i, Color2);
            }
        }
        show();
        Increment();
	}
	// ------- ^^ TheaterChase pattern ^^ -------

	// ------- vv FireWorks pattern vv -------
	void FireWorks(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = FIREWORKS;
        Interval = interval;
        TotalSteps = numPixels() - 1;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
	}
    
	void FireWorksUpdate()
	{
        for(int i=0; i< numPixels(); i++)
        {

            if ((i + Index) % random(3,6) == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
	}
	// ------- ^^ FireWorks pattern ^^ -------
	
	// ------- vv ScannerChase pattern vv -------
	void ScannerChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = SCANNER_CHASE;
        Interval = interval;
        TotalSteps = (numPixels() - 1);
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
	}

	void ScannerChaseUpdate()
	{
        for(int i=0; i< numPixels(); i++)
        {
            if (i == TotalSteps - Index)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
	}
	// ------- ^^ ScannerChase pattern ^^ -------
	
	// ------- vv ColorWipe pattern vv -------
	void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color;
        Index = 0;
          Direction = dir;
	}

	void ColorWipeUpdate()
	{
        setPixelColor(Index, Color1);
        show();
        Increment();
	}
	// ------- ^^ ColorWipe pattern ^^ -------
	
	// ------- vv Scanner pattern vv -------
	void Scanner(uint32_t color1, uint8_t interval)
	{
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 0;
	}

	void ScannerUpdate()
	{ 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
            }
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
	}
	// ------- ^^ Scanner pattern ^^ -------

	// ------- vv DirectionalScanner pattern vv -------
	void DirectionalScanner(uint32_t color1, uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = DIRECTIONAL_SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1);
        Color1 = color1;
        Index = 0;
        Direction = dir;
	}

	void DirectionalScannerUpdate()
	{ 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right      //
            {                                                //
                setPixelColor(i, Color1);                    //
                
            }                                                //
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
            if (i == TotalSteps){
                  setPixelColor(i, Color1);
                  setPixelColor((i - 1), DimColor(getPixelColor((i - 1))));
                  setPixelColor((i - 2), DimColor(getPixelColor((i - 2))));
            }
        }
        show();
        Increment();
	}
	// ------- ^^ DirectionalScanner pattern ^^ -------
	
	// ------- vv Fade pattern vv -------
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
	{
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
	}

	void FadeUpdate()
	{
        // Calculate linear interpolation between Color1 and Color2
        // Optimise order of operations to minimize truncation error
        uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
        uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
        uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
        
        ColorSet(Color(red, green, blue));
        show();
        Increment();
	}
	// ------- ^^ Fade pattern ^^ -------

	// ------- vv Pattern utilities vv -------
	// Calculate 50% dimmed version of a color (used by ScannerUpdate)
	uint32_t DimColor(uint32_t color)
	{
        // Shift R, G and B components one bit to the right
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
	}

	// Set all pixels to a color (synchronously)
	void ColorSet(uint32_t color)
	{
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
	}

	// Returns the Red component of a 32-bit color
	uint8_t Red(uint32_t color)
	{
        return (color >> 16) & 0xFF;
	}

	// Returns the Green component of a 32-bit color
	uint8_t Green(uint32_t color)
	{
        return (color >> 8) & 0xFF;
	}

	// Returns the Blue component of a 32-bit color
	uint8_t Blue(uint32_t color)
	{
        return color & 0xFF;
	}

	// Input a value 0 to 255 to get a color value.
	// The colours are a transition r - g - b - back to r.
	uint32_t Wheel(byte WheelPos)
	{
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
	}
	// ------- ^^ Pattern utilities ^^ -------
};

// hardware definitions
#define micpin   A5     //microphone pin
#define tiltpin  A0     //tilt sensor pin
#define hrmpin   A3     //HRM pin
#define micwin   10     //microphone sampling window
#define micfloor 10     //microphone analog read floor
#define micceil  250    //microphone analog read ceiling
const int buttonPin = 12;   // momentary switch hardware pin -- program cycler
const int largepin = 7;		// Long arm of cross
const int smallpin = 6;	  // short arm of cross


// global variable initialization
int val = 0;                 // brightness pot value
int brightLevel = 0;         // brightness level, mapped to pot value
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int buttonMax = 9;           // button counter max value, resets to 0 when value reached


// declare led objects
NeoPatterns large(7, largepin, NEO_GRB + NEO_KHZ800, &LargeComplete);
NeoPatterns small(6, smallpin, NEO_GRB + NEO_KHZ800, &SmallComplete);

const int NumComponents = 2;

NeoPatterns *Components[NumComponents];

void setup() {
  Serial.begin(115200);			// start serial
 // Initializes Timer2 to throw an interrupt every 2mS.
        TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
        TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER 
        OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
        TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
        sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED          
      } 


  Components[0] = &large;
  Components[1] = &small;

  for (int i=0; i<NumComponents; ++i)
  {
    Components[i]->begin();
    Components[i]->Scanner(Ring1.Color(255,0,0), 55);
  }
}

// hardware interrupt handler
ISR(TIMER2_COMPA_vect){
  


  // read pot value and map to brightness range; 0 to 1023 is the analog value, 0 to 100 is the mapped brightness value
  val = analogRead(micpin);
  brightLevel = map(val, 0, 1023, 1, 100);

  for (int i=0; i<NumComponents; ++i)
  {
    Components[i]->setBrightness(brightLevel);
    Components[i]->Update();
  }


  
}

void loop() {
    ChangePattern();                          // call the changepattern function.
}

void ChangePattern(){
  buttonPushCounter = random(9);
  if( buttonPushCounter ==  0)
  {
    Head.ActivePattern = COLOR_WIPE;
    Head.Interval = 50;
    Head.TotalSteps = Head.numPixels();
	
    ArmRight.ActivePattern = FIRE;
    ArmRight.Interval = 75;
    ArmRight.TotalSteps = ArmRight.numPixels();
	
    ArmLeft.ActivePattern = FIRE;
    ArmLeft.Interval = 75;
    ArmLeft.TotalSteps = ArmLeft.numPixels();
	
    Mohawk.ActivePattern = DIRECTIONAL_SCANNER;
    Mohawk.Interval = random(30,100);
    Mohawk.TotalSteps = Mohawk.numPixels();
    Mohawk.Direction = FORWARD;
	
    Sides.ActivePattern = DIRECTIONAL_SCANNER;
    Sides.Interval = random(30,100);
    Sides.TotalSteps = Sides.numPixels();
    Sides.Direction = FORWARD;
	
  }
  
  if( buttonPushCounter ==  1)
  {
    Head.ActivePattern = RAINBOW_CYCLE;
    Head.Interval = 5;
    Head.TotalSteps = 255;

    ArmLeft.ActivePattern = RAINBOW_CYCLE;
    ArmLeft.Interval = 5;
    ArmLeft.TotalSteps = 255;

    ArmRight.ActivePattern = RAINBOW_CYCLE;
    ArmRight.Interval = 5;
    ArmRight.TotalSteps = 255;
    ArmRight.Direction = REVERSE;
    
    Mohawk.ActivePattern = RAINBOW_CYCLE;
    Mohawk.Interval = 5;
    Mohawk.TotalSteps = 255;
    Mohawk.Direction = REVERSE;
    
    Sides.ActivePattern = RAINBOW_CYCLE;
    Sides.TotalSteps = 255;
    Sides.Interval = 5;
    Sides.Direction = REVERSE;
  }
  
  if( buttonPushCounter ==  2)
  {  
    Head.ActivePattern = FIRE;
    Head.Interval = 75;
    Head.TotalSteps = Head.numPixels();

    ArmLeft.ActivePattern = FIRE;
    ArmLeft.Interval = 75;
    ArmLeft.TotalSteps = ArmLeft.numPixels();

    ArmRight.ActivePattern = FIRE;
    ArmRight.Interval = 75;
    ArmRight.TotalSteps = ArmRight.numPixels();
    ArmRight.Direction = REVERSE;
	
    Mohawk.ActivePattern = RAINBOW_CYCLE;
    Mohawk.Interval = 3;
    Mohawk.TotalSteps = 255;
    Mohawk.Direction = FORWARD;
    
    Sides.ActivePattern = THEATER_CHASE;
    Sides.Interval = 105;
    Sides.TotalSteps = Sides.numPixels();
    Sides.Direction = REVERSE;
  }
  
  if( buttonPushCounter ==  3)
  {  
    Head.ActivePattern = FIRE;
    Head.Interval = 75;
    Head.TotalSteps = Head.numPixels();

    ArmLeft.ActivePattern = FIRE;
    ArmLeft.Interval = 75;
    ArmLeft.TotalSteps = ArmLeft.numPixels();

    ArmRight.ActivePattern = FIRE;
    ArmRight.Interval = 75;
    ArmRight.TotalSteps = ArmRight.numPixels();
    ArmRight.Direction = REVERSE;
	
    Mohawk.ActivePattern = RAINBOW_CYCLE;
    Mohawk.Interval = 1;
    Mohawk.TotalSteps = 255;
    Mohawk.Direction = FORWARD;
    
    Sides.ActivePattern = RAINBOW_CYCLE;
    Sides.Interval = 2;
    Sides.TotalSteps = 255;
    Sides.Direction = REVERSE;
  }
  
  if( buttonPushCounter ==  4)
  {
    Head.ActivePattern = RAINBOW_CYCLE;
    Head.Interval = 3;
    Head.TotalSteps = 255;

    ArmLeft.ActivePattern = THEATER_CHASE;
    ArmLeft.Interval = 75;
    ArmLeft.TotalSteps = ArmLeft.numPixels();

    ArmRight.ActivePattern = THEATER_CHASE;
    ArmRight.Interval = 75;
    ArmRight.TotalSteps = ArmRight.numPixels();
    ArmRight.Direction = REVERSE;

    Mohawk.ActivePattern = THEATER_CHASE;
    Mohawk.Interval = 75;
    Mohawk.TotalSteps = Mohawk.numPixels();
    Mohawk.Direction = REVERSE;    

    Sides.ActivePattern = THEATER_CHASE;
    Sides.Interval = 105;
    Sides.TotalSteps = Sides.numPixels();
    Sides.Direction = REVERSE;
  }

  if( buttonPushCounter ==  5)
  {
    Head.ActivePattern = FIRE;
    Head.Interval = 75;
    Head.TotalSteps = Head.numPixels();

    ArmLeft.ActivePattern = FIRE;
    ArmLeft.Interval = 75;
    ArmLeft.TotalSteps = ArmLeft.numPixels();
 
    ArmRight.ActivePattern = FIRE;
    ArmRight.Interval = 75;
    ArmRight.TotalSteps = ArmRight.numPixels();
    ArmRight.Direction = REVERSE;

    Mohawk.ActivePattern = WATER;
    Mohawk.Interval = 75;
    Mohawk.TotalSteps = Mohawk.numPixels();
    Mohawk.Direction = REVERSE;    

    Sides.ActivePattern = WATER;
    Sides.Interval = 75;
    Sides.TotalSteps = Sides.numPixels();
    Sides.Direction = REVERSE;
  }

  if( buttonPushCounter ==  6)
  {
    Head.ActivePattern = FIRE;
    Head.Interval = 75;
    Head.TotalSteps = Head.numPixels();

    ArmLeft.ActivePattern = FIRE;
    ArmLeft.Interval = 75;
    ArmLeft.TotalSteps = ArmLeft.numPixels();

    ArmRight.ActivePattern = FIRE;
    ArmRight.Interval = 75;
    ArmRight.TotalSteps = ArmRight.numPixels();
    ArmRight.Direction = REVERSE;

    Mohawk.ActivePattern = FIRE;
    Mohawk.Interval = 115;
    Mohawk.TotalSteps = Mohawk.numPixels();
    Mohawk.Direction = REVERSE;    

    Sides.ActivePattern = FIRE;
    Sides.Interval = 100;
    Sides.TotalSteps = Sides.numPixels();
    Sides.Direction = REVERSE;
  }
  if( buttonPushCounter ==  7)
  {
    Head.ActivePattern = WATER;
    Head.Interval = random(20,50);
    Head.TotalSteps = Head.numPixels();

    ArmLeft.ActivePattern = WATER;
    ArmLeft.Interval = random(20,50);
    ArmLeft.TotalSteps = ArmLeft.numPixels();

    ArmRight.ActivePattern = WATER;
    ArmRight.Interval = random(20,50);
    ArmRight.TotalSteps = ArmRight.numPixels();
    ArmRight.Direction = REVERSE;

    Mohawk.ActivePattern = WATER;
    Mohawk.Interval = random(20,50);
    Mohawk.TotalSteps = Mohawk.numPixels();
    Mohawk.Direction = REVERSE;    

    Sides.ActivePattern = WATER;
    Sides.Interval = random(20,50);
    Sides.TotalSteps = Sides.numPixels();
    Sides.Direction = REVERSE;
  }
  
  if( buttonPushCounter ==  8)
  {  
    
    Head.ActivePattern = FIRE;
    Head.Interval = 75;
    Head.TotalSteps = Head.numPixels();

    ArmLeft.ActivePattern = FIRE;
    ArmLeft.Interval = 75;
    ArmLeft.TotalSteps = ArmLeft.numPixels();

    ArmRight.ActivePattern = FIRE;
    ArmRight.Interval = 75;
    ArmRight.TotalSteps = ArmRight.numPixels();
    ArmRight.Direction = REVERSE;
	
    Mohawk.ActivePattern = RAINBOW_CYCLE;
    Mohawk.Interval = 5;
    Mohawk.TotalSteps = 255;
    
    Sides.ActivePattern = RAINBOW_CYCLE;
    Sides.Interval = 5;
    Sides.TotalSteps = 255;
  }

// placeholders for more patterns i guess
//  if( buttonPushCounter ==  7){ }
//  if( buttonPushCounter ==  8){ }
//  if( buttonPushCounter ==  9){ }
//  if( buttonPushCounter == 10){ }
//  if( buttonPushCounter == 11){ }
//  if( buttonPushCounter == 12){ }
//  if( buttonPushCounter == 13){ }
}

//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------

// Head Completion Callback
void SmallComplete()
{
      small.Color1 = Head.Wheel(random(255));
}

// Arm Completion Callback
void LargeComplete()
{
       large.Color1 = ArmLeft.Wheel(random(255));
       
}

void ArmRightComplete()
{
	//       ArmRight.Color1 = ArmLeft.Color1;
	//       ArmRight.Color1 = ArmRight.Wheel(random(255));
}

// Sides Completion Callback
void SidesComplete()
{
    // Random color change for next scan
    Sides.Color1 = Sides.Wheel(random(255));
}
	
// Mohawk Completion Callback
void MohawkComplete()
{
    // Random color change for next scan
    Mohawk.Color1 = Mohawk.Wheel(random(255));
}
