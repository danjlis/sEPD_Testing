<<<<<<< HEAD
# sEPD_Testing
=======
# sEPD_Testing

# The code directory holds the code and the data directory holds the data.

First thing to do is run the tests.

Edit the config file on the test_stand computer to be sector two.
Then run make_runs.cmd
Then run Full_Line_Test.cmd

This should take a two to 3 days.

Then take the data and put it into the Data/s#/ directory.

Go to code directory and change the Line_config.config and Full_config.config to be for sector # (whatever you just tested.

Then run Full_Test_Analysis.C
Then run Line_Test_Analysis.C
Then run Make_Health_Sheet.C(sector #)
Do not forget to add in the sector number for the last one.
