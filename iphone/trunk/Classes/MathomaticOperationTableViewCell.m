//
//  MathomaticOperationTableViewCell.m
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationTableViewCell.h"
#import "MathomaticOperation.h"


@implementation MathomaticOperationTableViewCell

@synthesize operation;
@synthesize height;
@synthesize delegate;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier delegate:(id<MathomaticOperationTableViewCellDelegate>)d
{
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
        operation = nil;
        height = 0;
        self.delegate = d;
    }
    return self;
}

- (void)setOperation:(MathomaticOperation*)op
{
    // remove all our current subviews
    [[self subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];
    
    [operation release];
    operation = [op retain];
    
    height = 3;
    
    if ([operation name] != nil)
        [self attachText:[operation name] withSize:14 andColor:[UIColor grayColor]];
    
    // attach the inputs
    [self attachEquations: [operation inputs]];
    
    // attach the intermediate steps and their titles
    for (int ii = 0; ii < [[operation steps] count]; ii ++)
    {
        NSArray * equations = [[operation steps] objectAtIndex: ii];
        NSString * name = [[operation stepNames] objectAtIndex: ii];
        
        [self attachText:name withSize:14 andColor:[UIColor grayColor]];
        [self attachEquations: equations];
    }
}

- (void)attachText:(NSString*)text withSize:(int)size andColor:(UIColor*)color
{
    UIFont * font = [UIFont systemFontOfSize: size];
    CGSize rect = [text sizeWithFont: font];
    
    UIImageView * divider = [[[UIImageView alloc] initWithImage:[UIImage imageNamed: @"divider.png"]] autorelease];
    [divider setFrame: CGRectMake(6, height+rect.height/2, 320, 2)];
    [self addSubview: divider];
    
    UILabel * nameLabel = [[[UILabel alloc] initWithFrame: CGRectMake((310-rect.width), height, rect.width, rect.height)] autorelease];
    [nameLabel setText: text];
    [nameLabel setFont: font];
    [nameLabel setTextColor: color];
    [self addSubview: nameLabel];

    height += rect.height;
}

- (void)attachEquations:(NSArray*)equations
{
    int padding = 3;
    int x = 0;
    NSMutableArray * buttons = [NSMutableArray array];
        
    for (MathomaticExpression * expression in equations){
        ExpressionButtonView * button = [[ExpressionButtonView alloc] initWithExpression: expression];
        [button addTarget:self action:@selector(expressionClicked:) forControlEvents:UIControlEventTouchUpInside];
        [buttons addObject: button];
        [self addSubview: button];
        [button release];
    }
    
    while (x < [buttons count]){
        int row_width = padding;
        int row_height = 0;
        int y = x;
        
        // scan ahead and determine how many will fit on this row
        while (y < [buttons count])
        {
            ExpressionButtonView * button = [buttons objectAtIndex: y];
            if (row_width + [[buttons objectAtIndex:y] frame].size.width > self.frame.size.width)
                break;
            
            row_height = fmax(row_height, [button frame].size.height);
            row_width += [button frame].size.width + padding;
            y++;
            
        }
        
        // actually place them on the row
        row_width = padding;
        while (x < y)
        {
            ExpressionButtonView * button = [buttons objectAtIndex: x];
            button.frame = CGRectMake(row_width, height + (row_height - button.frame.size.height)/2, button.frame.size.width, button.frame.size.height);
            row_width += button.frame.size.width + padding;
            x++;
            
        }
        height += row_height + padding;
    }
}

- (void)expressionClicked:(ExpressionButtonView*)sender
{
    [delegate mathomaticOperationTableViewCellClicked: [sender expression]];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    // Configure the view for the selected state
}

- (void)dealloc {
    [delegate release];
    [operation release];
    [super dealloc];
}


@end
