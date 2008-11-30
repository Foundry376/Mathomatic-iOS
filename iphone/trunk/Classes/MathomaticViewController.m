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
#import "MathomaticOperationTableViewCell.h"
#import "MathomaticOperationDerivative.h";
#import "MathomaticOperationFactor.h";
#import "MathomaticOperationUnfactor.h";
#import "MathomaticOperationLaplace.h";
#import "MathomaticOperationIntegral.h";
#import "MathomaticOperationSolve.h";
#import "MathomaticOperationSimplify.h";

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
    [commandHistory setFrame: CGRectMake(0,0, 320, 128)];
}

- (void)viewDidLoad 
{
    [super viewDidLoad];
    
    keyboardVisible = YES;
    [keyboardSlideButton setFrame: CGRectMake([keyboardSlideButton frame].origin.x, 417, [keyboardSlideButton frame].size.width, [keyboardSlideButton frame].size.height)];

    commandStack = [[NSMutableArray alloc] init];
    commandStackCells = [[NSMutableArray alloc] init];
    [commandHistory setSeparatorStyle: UITableViewCellSeparatorStyleNone];
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
    [commandStack release];
    [commandStackCells release];
    [super dealloc];
}

- (void)performCommand:(MathomaticOperation*)c
{
    NSString * error = [c perform];
    
    if (!error)
        [self addCommand: c];
    else {
        UIAlertView * v = [[UIAlertView alloc] initWithTitle:@"Error" message:error delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [v show];
        [v release];
    }
}

- (void)addCommand:(MathomaticOperation*)c
{
    [commandStack addObject: c];
    
    MathomaticOperationTableViewCell * row = [[[MathomaticOperationTableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"command" delegate: self] autorelease];
    [row setOperation: c];
    
    [commandStackCells addObject: row];
    [commandHistory reloadData];
    
    NSIndexPath * path = [NSIndexPath indexPathForRow: [commandStack count]-1 inSection:0];
    [commandHistory scrollToRowAtIndexPath:path atScrollPosition:UITableViewScrollPositionBottom animated:YES];
}


- (BOOL)addKeyboardEntry
{
// check the input and make sure parenthesis are balanced
    int               location = 0;
    int               depth = 0;
    NSMutableString * str = [keyboard field];
    
    if ([str length] == 0)
        return NO;
        
    while (location < [str length]){
        if ([[str substringWithRange:NSMakeRange(location, 1)] isEqualToString: @"("])
            depth ++;
        if ([[str substringWithRange:NSMakeRange(location, 1)] isEqualToString: @")"])
            depth --;
        location ++;
    }
    
    while (depth > 0){
        [str appendString:@")"];
        depth --;
    }

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

- (MathomaticExpression*)lastExpression
{
    return [[[commandStack lastObject] inputs] lastObject];
}

- (NSArray*)recentExpressions:(BOOL)equationsOnly unique:(BOOL)unique
{
    NSMutableArray * recent = [[NSMutableArray alloc] init];
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
    if (![keyboard fieldIsBlank])
        if (![self addKeyboardEntry])
            return;
        
    if ([commandStack count] == 0){
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Please enter one or more expressions first!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
    } else {
        // setup the operations sheet
        UIActionSheet * operationSheet = [[UIActionSheet alloc] initWithTitle:@"" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"Integral...", @"Derivative...", @"Solve...", @"Laplace...", @"Factor", @"Unfactor", @"Simplify", nil];
        [operationSheet showInView: self.view];
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
        op = [[MathomaticOperationFactor alloc] init];
    } else if (buttonIndex == 5){
        op = [[MathomaticOperationUnfactor alloc] init];
    } else if (buttonIndex == 6){
        op = [[MathomaticOperationSimplify alloc] init];
    }
    
    NSString * error = [op prepareWithDelegate: self];
    if (error != nil){
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Error" message:error delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
    }
}

#pragma mark MathomaticOperationTableViewCellDelegate functions

- (void)mathomaticOperationTableViewCellClicked:(MathomaticExpression*)expression
{
    [keyboard setField: [expression equationText]];
}

@end
