class TestClass
{
public:
  uint8_t Id;
  TestClass(uint8_t aId): Id(aId)
  {
    Serial.print("Constructor ");
    Serial.println(Id);
  };
  ~TestClass()
  {
    Serial.print("Destructor ");
    Serial.println(Id);
  };
};

void setup()
{
  Serial.begin(115200);
  Test();
}

void Test()
{
  TestClass c1 = TestClass(41);
  TestClass c2 = TestClass(42);
}

void loop()
{
}
