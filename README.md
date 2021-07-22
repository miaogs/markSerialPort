# markSerialPort
Mark Serial Port Tool






| Changes            | Signals                        | Slots              | ComboBox | Label | Seetings                      |
|--------------------|--------------------------------|--------------------|----------|-------|-------------------------------|
| Serial Port update | emit onNewSerialPort           | onNewPortList      | Yes      | No    | NO                            |
| ComboBox select    | QComboBox::currentIndexChanged | onChooseSerialPort | No       | Yes   | No                            |
| User(Mouse)        | emit  onLabeltoLineText        | onUpdateLabel      | No       | Yes   | Yes(New Mark or Label update) |


