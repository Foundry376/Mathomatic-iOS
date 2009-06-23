//
//  mathomaticViewController.m
//  mathomatic
//
//  Created by Ben Gotow on 10/23/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "MathomaticViewController.h"
#import "AboutViewController.h"

#import "MathomaticExpression.h"
#import "MathomaticOperation.h"
#import "MathomaticOperationDerivative.h"
#import "MathomaticOperationFactor.h"
#import "MathomaticOperationUnfactor.h"
#import "MathomaticOperationLaplace.h"
#import "MathomaticOperationTaylor.h"
#import "MathomaticOperationIntegral.h"
#import "MathomaticOperationSolve.h"
#import "MathomaticOperationSimplify.h"

#define kMaxStackLength 20

@implementation MathomaticViewController

@synthesize commandStack;

- (IBAction)aboutPressed:(id)sender
{
    AboutViewController * about = [[AboutViewController alloc] initWithNibName:@"AboutViewController" bundle:[NSBundle mainBundle]];
    [self presentModalViewController:about animated:YES];
    [about release];
}

- (IBAction)keyboardSlideToggle:(id)sender
{
    if (keyboardVisible){
        [UIView beginAnimations: nil context: nil];
        [UIView setAnimationDuration: 0.3];
        [UIView setAnimationCurve: UIViewAnimationCurveEaseOut];
        [UIView setAnimationDelegate: self];
        [keyboard setFrame: CGRectMake(0, 480, keyboard.frame.size.width, keyboard.frame.size.height)];
        [commandHistory setFrame: CGRectMake(0,0, 320, 462-[keyboardSlideButton frame].size.height)];
        [keyboardSlideButton setImage:[UIImage imageNamed: @"slideUp.png"] forState:UIControlStateNormal];
        [UIView commitAnimations];
    } else {
        [UIView beginAnimations: nil context: nil];
        [UIView setAnimationDuration: 0.3];
        [UIView setAnimationDelegate: self];
        [UIView setAnimationDidStopSelector:@selector(keyboardSlideUpComplete)];
        [keyboardSlideButton setImage:[UIImage imageNamed: @"slideDown.png"] forState:UIControlStateNormal];
        [keyboard setFrame: CGRectMake(0, 110, keyboard.frame.size.width, keyboard.frame.size.height)];
        [UIView commitAnimations];
    }
    
    keyboardVisible = !keyboardVisible;
}

- (void)keyboardSlideUpComplete
{
    [commandHistory setFrame: CGRectMake(0,0, 320, 126)];
}

- (void)viewDidLoad 
{
    [super viewDidLoad];
    
    keyboardVisible = YES;
    [keyboardSlideButton setFrame: CGRectMake([keyboardSlideButton frame].origin.x, 417, [keyboardSlideButton frame].size.width, [keyboardSlideButton frame].size.height)];

    // load the command stack from the saved archive, if one exists
    commandStack = [[NSKeyedUnarchiver unarchiveObjectWithFile: [[NSString stringWithString: @"~/Documents/commands.stack"] stringByExpandingTildeInPath]] retain];
    if (!commandStack)
        commandStack = [[NSMutableArray alloc] init];

    // create command stack cells for each existing item in the commandStack
    commandStackCells = [[NSMutableArray alloc] init];
    for (MathomaticOperation * operation in commandStack){
        MathomaticOperationTableViewCell * row = [[[MathomaticOperationTableViewCell alloc] initWithReuseIdentifier:@"command" delegate: self] autorelease];
        [row setOperation: operation];
        [commandStackCells addObject: row];
    }
    
    spinnerTimer = nil;
    [commandHistory setSeparatorStyle: UITableViewCellSeparatorStyleNone];

    // iPhone OS 3.0 only
    if ([commandHistory respondsToSelector:@selector(setAllowsSelection:)])
        [commandHistory setAllowsSelection: NO];
    if ([commandHistory respondsToSelector:@selector(setAllowsSelectionDuringEditing:)])
        [commandHistory setAllowsSelectionDuringEditing: NO];
    
    // create the table view header with the delete / edit buttons
    UIImageView * h = [[UIImageView alloc] initWithFrame: CGRectMake(0,0,320,49)];
    [h setImage: [UIImage imageNamed: @"header_background.png"]];
    [h setUserInteractionEnabled: YES];
    
    editButton = [[UIButton buttonWithType: UIButtonTypeCustom] retain];
    [editButton setFrame: CGRectMake(0,0,160,49)];
    [editButton setAdjustsImageWhenHighlighted: NO];
    [editButton setImage:[UIImage imageNamed:@"history_edit.png"] forState:UIControlStateNormal];
    [editButton setImage:[UIImage imageNamed:@"history_edit_down.png"] forState:UIControlStateHighlighted];
    [editButton setImage:[UIImage imageNamed:@"history_edit_selected.png"] forState:UIControlStateSelected];
    [editButton addTarget:self action:@selector(historyEdit) forControlEvents:UIControlEventTouchUpInside];
    
    clearButton = [[UIButton buttonWithType: UIButtonTypeCustom] retain];
    [clearButton setFrame: CGRectMake(160,0,160,49)];
    [clearButton setAdjustsImageWhenHighlighted: NO];
    [clearButton setImage:[UIImage imageNamed:@"history_clear.png"] forState:UIControlStateNormal];
    [clearButton setImage:[UIImage imageNamed:@"history_clear_down.png"] forState:UIControlStateHighlighted];
    [clearButton addTarget:self action:@selector(historyDelete) forControlEvents:UIControlEventTouchUpInside];
    
    [h addSubview: editButton];
    [h addSubview: clearButton];
    [commandHistory setTableHeaderView: h];
    [h autorelease];
    
    // if there are items in the stack, make sure we load them into the table 
    // and scroll to the bottom.
    if ([commandStack count] > 0){
        [commandHistory reloadData];
        NSIndexPath * path = [NSIndexPath indexPathForRow: [commandStack count]-1 inSection:0];
        [commandHistory scrollToRowAtIndexPath:path atScrollPosition:UITableViewScrollPositionBottom animated:NO];   
    }
}

- (void)save
{   
    // if we are not just showing a sheet, the application is probably closing.
    // save all the items in the commandStack
    if (self.modalViewController == nil)
        [NSKeyedArchiver archiveRootObject:commandStack toFile:[[NSString stringWithString: @"~/Documents/commands.stack"] stringByExpandingTildeInPath]];
}


- (void)historyDelete
{
    UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Are you sure?" message:@"Are you sure you want to clear the history?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Clear", nil];
    [alert show];
    [alert release];
}

- (void)historyDeleteConfirmed
{
    [commandStack removeAllObjects];
    [commandStackCells removeAllObjects];
    [commandHistory reloadData];
    
    if ([commandHistory isEditing])
        [self historyEdit];
}

- (void)historyEdit
{    
    [commandHistory setEditing:![commandHistory isEditing] animated:YES];
    [editButton setSelected: [commandHistory isEditing]];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
}

- (void)dealloc 
{
    [clearButton release];
    [editButton release];
    [commandStack release];
    [commandStackCells release];
    [super dealloc];
}

- (void)performCommand:(MathomaticOperation*)c
{
    [spinnerTimer invalidate];
    [spinnerTimer release];
    spinnerTimer = [[NSTimer scheduledTimerWithTimeInterval: 0.5 target:spinner selector:@selector(startAnimating) userInfo:nil repeats:NO] retain];
    [NSThread detachNewThreadSelector:@selector(performCommandThreaded:) toTarget:self withObject:c];
        
}

- (void)performCommandThreaded:(MathomaticOperation*)c
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSString * error = [c perform];
    NSDictionary * dict = [NSMutableDictionary dictionaryWithObject:c forKey:@"operation"];
    if (error != nil)
        [dict setValue:error forKey:@"error"];
        
    [self performSelectorOnMainThread:@selector(performCommandDone:) withObject:dict waitUntilDone:YES];
    [pool release];
}

- (void)performCommandDone:(NSDictionary*)result
{
    MathomaticOperation * c = [result objectForKey:@"operation"];
    
    if (![result objectForKey: @"error"])
        [self addCommand: c];
    else {
        UIAlertView * v = [[UIAlertView alloc] initWithTitle:@"Error" message:[result objectForKey: @"error"] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [v show];
        [v release];
    }
    [spinnerTimer invalidate];
    [spinnerTimer release];
    spinnerTimer = nil;
    
    [spinner stopAnimating];
}

- (void)addCommand:(MathomaticOperation*)c
{
    // if we're editing, stop editing
    if ([commandHistory isEditing])
        [self historyEdit];
        
    // add the command to the command stack and remove the first object if there are more than kMaxStackLength
    [commandStack addObject: c];
    if ([commandStack count] > kMaxStackLength)
        [commandStack removeObjectAtIndex: 0];
        
    // create a new row to represent the operation and keep it cached for future use. We need to keep
    // this array ourselves (and not just let the TableView manage it as usual) so that we can respond to
    // heightForRowAtIndexPath without calling cellForIndexPath (which causes an infinite loop... apparently.)
    MathomaticOperationTableViewCell * row = [[[MathomaticOperationTableViewCell alloc] initWithReuseIdentifier:@"command" delegate: self] autorelease];
    [row setOperation: c];
    
    // add the item. If there are more than kMaxStackLength, remove the first object 
    [commandStackCells addObject: row];
    if ([commandStackCells count] > kMaxStackLength)
        [commandStackCells removeObjectAtIndex: 0];
    [commandHistory reloadData];
    
    // scroll to the new item in the tableView
    NSIndexPath * path = [NSIndexPath indexPathForRow: [commandStack count]-1 inSection:0];
    [commandHistory scrollToRowAtIndexPath:path atScrollPosition:UITableViewScrollPositionBottom animated:YES];
}


- (BOOL)addKeyboardEntry
{
    int               location = 0;
    int               depth = 0;
    NSMutableString * str = [keyboard field];
    
    if ([str length] == 0)
        return NO;
        
    // count the number of parenthesis
    while (location < [str length]){
        if ([[str substringWithRange:NSMakeRange(location, 1)] isEqualToString: @"("])
            depth ++;
        if ([[str substringWithRange:NSMakeRange(location, 1)] isEqualToString: @")"])
            depth --;
        location ++;
    }
    
    // add as many close parenthesis as we need to make the expression balanced
    while (depth > 0){
        [str appendString:@")"];
        depth --;
    }

    // crate the expression object and determine if it valid. If it is, perform it and clear
    // the keyboard text. Otherwise, we'll display an error message.
    MathomaticExpression * expression = [MathomaticExpression expressionWithEquationText: str];
    if ([expression isValidExpression]){
        MathomaticOperation * operation = [[[MathomaticOperation alloc] init] autorelease];
        [operation addInput: expression];
        [self performCommand: operation];
        [keyboard clear];
        return YES;
    } else {
        UIAlertView * view = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Please enter a valid expression." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [view show];
        [view release];
        return NO;
    }
}

// fetches the last expression. This is called by operations when they are running
- (MathomaticExpression*)lastExpression
{
    if (([commandStack count] > 0) && ([[[commandStack lastObject] inputs] count] > 0))
        return [[[commandStack lastObject] inputs] lastObject];
    else
        return nil;
}

// fetches 10 recent expressions. Right now, this is only used for the solve command, and
// the options are always the same. They are just provided for future use.
- (NSArray*)recentExpressions:(BOOL)equationsOnly unique:(BOOL)unique
{
    NSMutableArray * recent = [NSMutableArray array];
    for (int ii = [commandStack count] - 1; ii >= 0; ii --)
    {
        MathomaticOperation * op = [commandStack objectAtIndex: ii];
        NSMutableArray * expressionSets = [NSMutableArray arrayWithArray: [op steps]];
        [expressionSets addObject: [op inputs]];
        
        for (NSArray * set in expressionSets){
            for (MathomaticExpression * expression in set){
                if ((([expression isEquation]) || (!equationsOnly)) && ((![recent containsObject: expression]) || (!unique)))
                {
                        [recent addObject: expression];                        
                        if ([recent count] == 10)
                            return recent;
                }

            }
        }
    }
    
    return recent;
}

#pragma mark Keyboard Delegate

- (void)keyboardEntryComplete
{
    [self addKeyboardEntry];
}

- (void)keyboardEntryPerform
{
    // if there is text in the keyboard, attempt to add it to the commandHistory
    if (![keyboard fieldIsBlank])
        if (![self addKeyboardEntry])
            return;
    
    // make sure you can't open the math sheet without entering an expression first. This would
    // effectively do nothing, and none of the operations would actually work.
    if ([commandStack count] == 0){
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Please enter one or more expressions first!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
    } else {
        // if we're editing, stop editing
        if ([commandHistory isEditing])
            [self historyEdit];
            
        // display the operations sheet
        UIActionSheet * operationSheet = [[UIActionSheet alloc] initWithTitle:@"" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"Integral...", @"Derivative...", @"Solve...", @"Laplace...", @"Taylor...", @"Factor", @"Unfactor", @"Simplify", nil];
        [operationSheet showInView: self.view];
        
        // determine if the sheet contains a table view (OS 3.0)
        BOOL isOS3 = NO;
        for (UIView * subview in [operationSheet subviews]){
            for (UIView * subview2 in [subview subviews])
                if ([subview2 isKindOfClass: [UITableView class]])
                    isOS3 = YES;
        }
        if (!isOS3){
            int y = 52;
            for (UIView * subview in [operationSheet subviews]){
                CGRect f = [subview frame];
                f.origin.y = y;
                y += f.size.height + 6;
                [subview setFrame: f];
            }
        }
        [operationSheet release];
        
    }
}

#pragma mark TableView Data Source

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return [commandStackCells objectAtIndex: [indexPath indexAtPosition: 1]];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return [(MathomaticOperationTableViewCell*)[commandStackCells objectAtIndex: [indexPath indexAtPosition: 1]] height];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [commandStack count];
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    return nil;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    [commandStack removeObjectAtIndex: [indexPath indexAtPosition: 1]];
    [commandStackCells removeObjectAtIndex: [indexPath indexAtPosition: 1]];
    
    // if there are no more rows, automatically leave the edit mode
    if ([commandStackCells count] == 0)
        [self historyEdit];
        
    [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject: indexPath] withRowAnimation:UITableViewRowAnimationLeft];
}

#pragma mark ActionSheet Delegate Functions

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    MathomaticOperation * op = nil;
    
    if (buttonIndex == 0){
        op = [[MathomaticOperationIntegral alloc] init];
    } else if (buttonIndex == 1){
        op = [[MathomaticOperationDerivative alloc] init];
    } else if (buttonIndex == 2){
        op = [[MathomaticOperationSolve alloc] init];
    } else if (buttonIndex == 3){
        op = [[MathomaticOperationLaplace alloc] init];
    } else if (buttonIndex == 4){
        op = [[MathomaticOperationTaylor alloc] init];
    } else if (buttonIndex == 5){
        op = [[MathomaticOperationFactor alloc] init];
    } else if (buttonIndex == 6){
        op = [[MathomaticOperationUnfactor alloc] init];
    } else if (buttonIndex == 7){
        op = [[MathomaticOperationSimplify alloc] init];
    } else if (buttonIndex == 8)
        return;
    
    NSString * error = [op prepareWithDelegate: self];
    if (error != nil){
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Error" message:error delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
        [op release];
    }
}

#pragma mark UIAlertView Delegate Functions

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 1)
        [self historyDeleteConfirmed];
}

#pragma mark MathomaticOperationTableViewCellDelegate functions

- (void)mathomaticOperationTableViewCellClicked:(MathomaticExpression*)expression
{
    [keyboard setField: expression];
}

@end
