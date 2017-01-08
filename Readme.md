#ExpenseBalancer
Welcome to ExpenseBalancer, a quick tool to optimize balance transfers when expense sharing is nasty.
It is designed to make the least amount of transfers between participants

##Command for Main menu
### add [options] [arguments]
    options: 
        -p: add participants to the pool, arguments are names which must be separated by space/spaces.

        -e: create a shared expense report, if no -a option specified this will enter into a expense session.
            arguments are [name of expense] [creditor] [amount] [participants...]
            if no participants specified, everyone are assumed as the participants of this expense
            example:
                add -e Dining Reagan 412.2 Carter Clinton Bush Trump 

        -a: add expense and commit right away, assume all participants split the charge 
            arguments are [name of the expense] [creditor] [amount] [participants...]
            the only different between this and -e is that it will still remain in the main menu

        -v: do it verbosely

### rm [options] [arguments]
    options:
        -p: remove participants from the pool, arguments are names which must be separated by space/spaces

        -e: remove a expense session by its name, if duplicates are found, a prompt will show up for confirmation

        -v: do it verbosely

### show [options] [arguments]
    options:
        -p: show all participants in the pool, no arguments needed

        -e: show all the expenses added, with details (share weight, amount, creditor), no arguments needed

        -t: show all the transfer information, if the result is not valid, an error will prompt
            use it after the "opt" command

            arguments are the name/names of the person making transfers

        -v: change output to be inversely sorted, combine it with the other three.

### opt [options] [arguments]
    options:
        -l: lazily optimize balance transfers, only minimum total amount of transfer dollars is guaranteed (default)

        -e: eagerly optimize balance transfers, the total number of transfer and total amount of transfer dollars
            are guaranteed. This posed a hard upper limit of 64 participants and it will run significantly slower when
            number of participants topped.
### undo
    undo the last change (add, or remove)

### help
    print this message

### quit
    quit ExpenseBalancer


##Commands for single Expense session
### add [options] [arguments]
    options:
        -p: add participants to this expense, the share weight is default to be 1, arguments are names separated by space.

        -v: add verbosely

### rm [options] [arguments]
    options:
        -p: remove participants to this expense, arguments are names separated by space.

        -v: add verbosely

### cg [options] [arguments]
    options:
        -w: change weights, the arguments are [name weight] pair/pairs, which signify the share weight after change
            example:
            before: Reagan(1), Clinton(1), Trump(1), Carter(1)
            do:     cg -w Reagan 0 Trump 5
            after:  Clinton(1), Trump(5), Carter(1)
            Any name that is not in the main menu pool is ignored
            Any weight down to 0 will be the same as rm -p

        -m: change the amount of this expense, argument is a decimal number.

        -n: change the name of this expense, argument is the name.

        -c: change the creditor of this expense, argument is the name of the creditor.

### show [options] [arguments]
    options:
        -p: show all participants along with their share weights in this expense.

        -m: show the amount of this expense.

        -n: show name of this expense.

        -c: show creditor of this expense.

        -s: show summary of this expense.

### commit
    commit this expense to the main menu, will return to the main menu upon successful commit

### help
    print this message

### quit
    abort this expense and return to the main menu


