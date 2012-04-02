//
//  EquationSelectController.m
//  mathomatic
//
//  Created by Ben Gotow on 11/13/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationSelectController.h"
#import "MathomaticExpressionTableViewCell.h"

@implementation EquationSelectController

@synthesize delegate;
@synthesize equations;

- (id)initWithEquations:(NSArray*)e andOperation:(NSObject<EquationSelectDelegate>*)d
{
    if (self = [super initWithStyle:UITableViewStyleGrouped]) {
    
        self.delegate = d;
        
        equations = [[NSMutableArray alloc] init];
        equationCells = [[NSMutableArray alloc] init];
        for (MathomaticExpression * expression in e)
        {
            MathomaticExpressionTableViewCell * cell = [[MathomaticExpressionTableViewCell alloc] initWithReuseIdentifier:@"mathomatic_cell"];
            [cell setExpression: expression];
            [equations addObject: expression];
            [equationCells addObject: cell];
            [cell release];
        }
        
        activeEquations = [[NSMutableArray alloc] init];
        [activeEquations addObject: [equations objectAtIndex: 0]];
        
        variables = [[NSMutableArray alloc] init];
        [self reloadVariables];
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


- (void)reloadVariables
{
    [variables removeAllObjects];
    activeVariable = -1;
    
    for (MathomaticExpression * c in activeEquations)
        for (NSString * var in [c equationVariables])
            if (![variables containsObject: var])
                [variables addObject: var];
                
    
    if ([variables count] > 0)
        activeVariable = 0;
}

- (void)cancel
{
    [delegate equationSelectCancelled];
    [self dismissModalViewControllerAnimated:YES];
}

- (void)solve
{
    if (activeVariable == -1){
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Please select a variable to solve for." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
    } else {
        [delegate equationsSelected: activeEquations solveVariable: [variables objectAtIndex: activeVariable]];
        [self dismissModalViewControllerAnimated:YES];
    }
}

- (void)dealloc {
    [equations release];
    [equationCells release];
    
    [activeEquations release];
    [variables release];
    [delegate release];
    
    [super dealloc];
}

#pragma mark Table View Data Source / Delegate Functions

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    if (section == 0)
        return [equations count];
    else
        return [variables count];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if ([indexPath indexAtPosition: 0] == 0)
        return [[equationCells objectAtIndex: [indexPath indexAtPosition: 1]] height];
    else
        return 38;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (section == 0)
        return @"Select one or more equations:";
    else
        return @"Solve for:";
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    int index = [indexPath indexAtPosition: 1];
    
    if ([indexPath indexAtPosition: 0] == 0)
    {
        MathomaticExpressionTableViewCell * cell = [equationCells objectAtIndex: index];
        if ([activeEquations containsObject: [equations objectAtIndex: index]])
            [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
        else
            [cell setAccessoryType: UITableViewCellAccessoryNone];
        return cell;
    }
    else
    {
        UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier: @"variable_cell"];
        if (cell == nil)
            cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"variable_cell"] autorelease];
    
        NSString * c = [variables objectAtIndex: index];
        if (activeVariable == index)
            [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
        else
            [cell setAccessoryType: UITableViewCellAccessoryNone];
        [cell.textLabel setText: c];
        return cell;
    }
    
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    UITableViewCell * cell = [tableView cellForRowAtIndexPath: indexPath];
    int section = [indexPath indexAtPosition: 0];
    int index = [indexPath indexAtPosition: 1];
    [cell setSelected: NO];
    
    if (section == 0)
    {
        if ([cell accessoryType] == UITableViewCellAccessoryNone){
            [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
            [activeEquations addObject: [equations objectAtIndex: index]];
        }else{
            [cell setAccessoryType: UITableViewCellAccessoryNone];
            [activeEquations removeObject: [equations objectAtIndex: index]];
        }
        
        [self reloadVariables];
        [[self tableView] reloadData];
    }
    else
    {
        [[tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:activeVariable inSection:1]] setAccessoryType: UITableViewCellAccessoryNone];
        
        activeVariable = index;
        [cell setAccessoryType: UITableViewCellAccessoryCheckmark];
    }
}


@end

