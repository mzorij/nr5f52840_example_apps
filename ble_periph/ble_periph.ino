#if defined(USE_TINYUSB)
#include <Adafruit_TinyUSB.h> // for Serial
#endif

#include <Arduino.h>

#include <bluefruit.h>
#include <utility/bonding.h>

uint8_t loop_count=0;

//BLEUart bleuart; // uart over ble
// TODO: examine BLEAdafruitSensor.h, refer to BLEAdafruitButton
BLEDis  bledis;  // device information

void setup() 
{
  // put your setup code here, to run once:

  Serial.begin(115200);

  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Setup complete");

  Serial.println("Initializing bond");
  // Note: this creates the bond directories in the file system
  bond_init();
  // Note: this throws an assertion if the list is empty it seems
  //Serial.println("Printing current bond list");
  //bond_print_list(BLE_GAP_ROLE_PERIPH);

  Bluefruit.begin();
  // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
  Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  // Configure and Start Device Information Service
  bledis.setManufacturer("Z Industries");
  bledis.setModel("crazy_mofo");
  bledis.begin();

  // prevent man-in-the-middle attacks
  //Bluefruit.Security.setMITM( true );
  // TODO: do I need this?
  //Bluefruit.Security.begin();

  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  //Bluefruit.Periph.begin();

  // Set connection secured callback, invoked when connection is encrypted
  Bluefruit.Security.setSecuredCallback(connection_secured_callback);

  // Configure and Start BLE Uart Service
  //bleuart.begin();

  // Set up and start advertising
  startAdv();
}

void startAdv(void)
{
  Serial.println("Starting adveristing");
  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  
  // Include bleuart 128-bit uuid
  //Bluefruit.Advertising.addService(bleuart);
  
  // Include CTS client UUID
  //Bluefruit.Advertising.addService(bleCTime);

  // Includes name
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}


void connect_callback(uint16_t conn_handle)
{
  BLEConnection* p_conn = Bluefruit.Connection(conn_handle);
  char buf[100];
  
  Serial.println("Connected");

  // Display some basic info on the connection
  Serial.print("role is ");
  Serial.println( p_conn->getRole() );

  Serial.print("peer name is ");
  p_conn->getPeerName( buf, 100 );
  Serial.println( buf );

  // saveCccd() - TODO? CCCD: client configuration descriptor 

  // request Pairing if not bonded
  if( p_conn->secured() )
  {
    Serial.println("Attempting to PAIR with the device, please press PAIR on your phone ... ");
    p_conn->requestPairing();
  }
  else
  {
    Serial.println("Connection already secured");
  }
  /*
  Serial.print("Discovering CTS ... ");
  if ( bleCTime.discover(conn_handle) )
  {
    Serial.println("Discovered");
    
    // Current Time Service requires pairing to work
    // request Pairing if not bonded
    Serial.println("Attempting to PAIR with the iOS device, please press PAIR on your phone ... ");
    conn->requestPairing();
  }*/
  
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}

void connection_secured_callback(uint16_t conn_handle)
{
  BLEConnection* p_conn = Bluefruit.Connection(conn_handle);

  if ( !p_conn->secured() )
  {
    Serial.println("In secure CB but not yet secured");
    // It is possible that connection is still not secured by this time.
    // This happens (central only) when we try to encrypt connection using stored bond keys
    // but peer reject it (probably it remove its stored key).
    // Therefore we will request an pairing again --> callback again when encrypted
    p_conn->requestPairing();
  }
  else
  {
    Serial.println("Secured");

  }
}
void loop() 
{
  // put your main code here, to run repeatedly:
  delay( 5000 );
  Serial.print( "Loop count is: ");
  Serial.println( loop_count );
  loop_count++;
}
