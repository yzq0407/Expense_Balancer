#ExpenseBalancer
Welcome to ExpenseBalancer, a quick tool to optimize balance transfers when expense sharing is nasty
It is designed to make the least amount of transfers between participants

##Options in Main menu
### --add-p [names...]
Add participants to the pool, names must be separated by space/spaces

### --rm-p [names...]
Remove participants from the pool, names must be separated by space/spaces

### --add-exp [creditor] [amount] [names of participants...]
Create a shared expense report session, name of creditor(whoever paied the expense) must be provided
The amount is default to be 0 if not given any value
names of participants are the people engaged in this expense activity, should be seprated by space/spaces
If the names of participants are not given, all possible participants (added by "--add-p") are assumed
Note that the creditor is always default to be added as one of the participants.

### --rm-exp
Remove the last added expense report, this is the rollback option for "--add-exp"

### --verbose
Make it verbose (not supported yet)

### --show [expense/partic/balance]
Show expense report / names of participants / balance information

### --opt 
Optimize balance transfer! Will print out a detailed suggested list of balance transfers

### --help
Print this message

### --quit
Quit AccountBalancer

##Options for single Expense session
### --add-p [names...]
Add participants for this shared expense, names must be separated by space/spaces

### --rm-p [names...]
Remove participants for this shared expense, names must be sepated by space/spaces
There must be at least one participants left

### --cw [name1, weight1, name2, weight2....]
Change the weight of the specified participant, all entry must be seprated by space/spaces
Weight has to be nonegative integer (if 0, then delete from the list of participants)

### --show [weights/partic/all/creditor/note]
Show different informations of this expense session

### --commit
Commit and add this expense to the list of expenses, then quit this session.

### --help
Print this message

### --quit
Aborted this expense and return back to main menu

