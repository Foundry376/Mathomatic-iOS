//
//  mathomaticViewController.m
//  mathomatic
//
//  Created by Ben Gotow on 10/23/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "MathomaticViewController.h"
#import "EquationTableViewCell.h"

@implementation MathomaticViewController

@synthesize commandStack;

- (void)viewDidLoad 
{
    [super viewDidLoad];
    commandStack = [[NSMutableArray alloc] init];
    
    [commandField setEditable: YES];
    [commandField setFontSize: 21];
    [commandField setup];
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
    [super dealloc];
}

- (void)performString:(NSString*)entry
{
    MathCommand * c = [[MathCommand alloc] init];
    [c addInput: @"clear all"];
    [c addInput: entry];
    [self performCommand: c];
    [c release];
}


- (void)performCommand:(MathCommand*)c
{
    if ([c run]){
        if ([c output])
            [self addCommand: c];
    } else {
        UIAlertView * v = [[UIAlertView alloc] initWithTitle:@"Error" message:[c output] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [v show];
        [v release];
    }
}

- (void)addCommand:(MathCommand*)c
{
    [commandStack addObject: c];
    
    [commandField setEquation:@""];
    [commandHistory reloadData];
    
    NSIndexPath * path = [NSIndexPath indexPathForRow: [commandStack count]-1 inSection:0];
    [commandHistory scrollToRowAtIndexPath:path atScrollPosition:UITableViewScrollPositionBottom animated:YES];
}


#pragma mark Keyboard Delegate

- (void)keyboardEntryComplete:(NSString*)entry
{
    // check the input and make sure parenthesis are balanced
    int location = 0;
    int depth = 0;
    while (location < [entry length]){
        if ([[entry substringWithRange:NSMakeRange(location, 1)] isEqualToString: @"("])
            depth ++;
        if ([[entry substringWithRange:NSMakeRange(location, 1)] isEqualToString: @")"])
            depth --;
        location ++;
    }
    
    while (depth > 0){
        entry = [entry stringByAppendingString:@")"];
        depth --;
    }

    [self performString: entry];
}

- (void)keyboardEntryPerform:(NSString*)entry
{
    // setup the operations sheet
    UIActionSheet * operationSheet = [[UIActionSheet alloc] initWithTitle:@"" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"Integral...", @"Derivative...", @"Solve...", @"Laplace...", @"Factor", @"Unfactor",nil];
    [operationSheet showInView: self.view];
    [operationSheet release];
}

#pragma mark TableView Data Source

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    EquationTableViewCell * c = (EquationTableViewCell*)[tableView dequeueReusableCellWithIdentifier: @"command"];
    if (!c) 
        c = [[[EquationTableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"command"] autorelease];
    
    [c setCommand: [commandStack objectAtIndex: [indexPath indexAtPosition: 1]]];
    return c;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return [[commandStack objectAtIndex:[indexPath indexAtPosition: 1]] attachedHeight]+6;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [commandStack count];
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [commandField setEquation: [[commandStack objectAtIndex: [indexPath indexAtPosition: 1]] output]];
    return nil;
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    Operation * op = nil;
    NSString * currentEquation = [commandField equation];
        
    if (buttonIndex == 0){
        op = [[OperationIntegrate alloc] initWithDelegate: self andEntry: currentEquation];
    } else if (buttonIndex == 1){
        op = [[OperationDerivative alloc] initWithDelegate: self andEntry: currentEquation];
    } else if (buttonIndex == 2){
        op = [[OperationSolve alloc] initWithDelegate: self andEntry: currentEquation];
    } else if (buttonIndex == 3){
        op = [[OperationLaplace alloc] initWithDelegate: self andEntry: currentEquation];
    } else if (buttonIndex == 4){
        op = [[OperationFactor alloc] initWithDelegate: self andEntry: currentEquation];
    } else if (buttonIndex == 5){
        op = [[OperationUnfactor alloc] initWithDelegate: self andEntry: currentEquation];
    }
    
    [op run];
}

@end
