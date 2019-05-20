temp = 0
screen = local_screen('Title') do
'
  SCREEN AUTO AUTO 0.1 FIXED

  VERTICAL
    TITLE "Local Variable and Target Display"

    VERTICALBOX
      LABELVALUE LOCAL LOCAL temp
      LABELVALUE INST ADCS POSX
      LABELVALUE INST ADCS TIMEFORMATTED WITH_UNITS 30
    END
  END
'
end

disable_instrumentation do
  5000000.times do
    temp += 1
  end
end

screen.close

temp2 = 5000000
screen_def =
'
  SCREEN AUTO AUTO 0.1 FIXED
  STAY_ON_TOP

  VERTICAL
    TITLE "Local Variable and Target Display #2"

    VERTICALBOX
      LABELVALUE LOCAL LOCAL temp2
      LABELVALUE INST ADCS POSY
      LABELVALUE INST ADCS TIMEFORMATTED WITH_UNITS 30
    END
  END
'

local_screen('Title2', screen_def, 100, 200)

disable_instrumentation do
  5000000.times do
    temp2 -= 1
  end
end

close_local_screens()
