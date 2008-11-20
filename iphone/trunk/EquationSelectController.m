//
//  EquationSelectController.m
//  mathomatic
//
//  Created by Ben Gotow on 11/13/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationSelectController.h"
#import "EquationTableViewCell.h"
#import "MathCommand.h"

@implementation EquationSelectController

@synthesize delegate;

- (id)initWithEquations:(NSArray*)commandHistory andOperation:(OperationSolve*)d
{
    if (self = [super initWithStyle:UITableViewStyleGrouped]) {
    
        self.delegate = d;
        
        equations = [[NSMutableArray alloc] init];
        int count = [commandHistory count];
        for (int ii = count-1; ii >= fmax(0, count - 8); ii--){
            MathCommand * c = [commandHistory objectAtIndex: ii];
            [equations addObjectsFromArray: [c outputEquations]];
        }
        
        if ([equations count] == 0)
        {
            UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Cannot Solve" message:@"Please enter one or more equations before attempting to solve." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [alert show];
            [alert release];
            return nil;
        }
        
        possibleVariables = [[NSArray alloc] initWithObjects: @"x", @"y", @"z", nil];
        visibleVariables = [[NSMutableArray alloc] init];
        activeVariables = [[NSMutableArray alloc] init];
        activeEquations = [[NSMutableArray alloc] init];
        
        [activeEquations addObject: [equations objectAtIndex: 0]];
        [self reloadVariables];
        [activeVariables addObject: [visibleVariables objectAtIndex: 0]];
    }
    return self;
}

// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {

    UIView * container = [[UIView alloc] initWithFrame: CGRectMake(10, 0, 300, 80)];

    UIButton * cancel = [UIButton buttonWithType: UIButtonTypeRoundedRect];
    [cancel setTitle:@"Cancel" forState:UIControlStateNormal];
    [cancel addTarget:self action:@selector(cancel) forControlEvents:UIControlEventTouchUpInside];
    [cancel setFrame: CGRectMake(0, 20, 140, 40)];
    [container addSubview: cancel];

    UIButton * solve = [UIButton buttonWithType: UIButtonTypeRoundedRect];
    [solve setTitle:@"Solve" forState:UIControlStateNormal];
    [solve addTarget:self action:@selector(solve) forControlEvents:UIControlEventTouchUpInside];
    [solve setFrame: CGRectMake(160, 20, 140, 40)];
    [container addSubview: solve];
        
    [[self tableView] setTableFooterView: container];
    [super viewDidLoad];
}


- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (section == 0)
        return [equations count];
    else
        return [visibleVariables count];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (section == 0)
        return @"Select one or more equations:";
    else
        return @"Solve for:";
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    EquationTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[EquationTableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
    }
    
    [cell setAccessoryType: UITableViewCellAccessoryNone];
    [cell setSelectionStyle: UITableViewCellSelectionStyleNone];
    if ([indexPath indexAtPosition: 0] == 0)
    {
        EquationView * c = [equations objectAtIndex: [indexPath indexAtPosition:1]];
        [cell setEquation: c];
        if ([activeEquations containsObject: c])
            [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
    }
    else
    {
        NSString * c = [visibleVariables objectAtIndex: [indexPath indexAtPosition: 1]];
        [cell setEquationText: c];
        if ([activeVariables containsObject: c])
            [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
    }
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    UITableViewCell * cell = [tableView cellForRowAtIndexPath: indexPath];
    
    if ([indexPath indexAtPosition: 0] == 0)
    {
        if ([cell accessoryType] == UITableViewCellAccessoryNone){
            [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
            [activeEquations addObject: [equations objectAtIndex: [indexPath indexAtPosition: 1]]];
        }else{
            [cell setAccessoryType: UITableViewCellAccessoryNone];
            [activeEquations removeObject: [equations objectAtIndex: [indexPath indexAtPosition: 1]]];
        }
        
        [self reloadVariables];
        [[self tableView] reloadData];
    }
    else
    {
        if ([cell accessoryType] == UITableViewCellAccessoryNone){
            [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
            [activeVariables addObject: [visibleVariables objectAtIndex: [indexPath indexAtPosition: 1]]];
        }else{
            [cell setAccessoryType: UITableViewCellAccessoryNone];
            [activeVariables removeObject: [visibleVariables objectAtIndex: [indexPath indexAtPosition: 1]]];
        }
    }
}

- (void)reloadVariables
{
    NSMutableArray * remaningVariables = [NSMutableArray arrayWithArray: possibleVariables];
    [visibleVariables removeAllObjects];
    for (EquationView * c in activeEquations){
        for (int x = [remaningVariables count] - 1; x >= 0; x--){
            NSString * var = [remaningVariables objectAtIndex: x];
            if ([[c equation] rangeOfString: var].location != NSNotFound){
                [visibleVariables addObject: var];
                [remaningVariables removeObject: var];
            }
        }
    }
    [activeVariables removeObjectsInArray: remaningVariables];
}

- (void)cancel
{
    [self.parentViewController dismissModalViewControllerAnimated:YES];
}

- (void)solve
{
    [delegate equationsSelected: activeEquations solveVariables: activeVariables fromVariables: visibleVariables];
    [self.parentViewController dismissModalViewControllerAnimated:YES];
}

- (void)dealloc {
    [equations release];
    [possibleVariables release];
    [visibleVariables release];
    [super dealloc];
}


@end

